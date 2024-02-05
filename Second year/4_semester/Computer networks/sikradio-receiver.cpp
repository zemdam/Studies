#include <cinttypes>
#include <condition_variable>
#include <endian.h>
#include <iostream>
#include <mutex>
#include <poll.h>
#include <pthread.h>
#include <regex>
#include <thread>
#include <tuple>
#include <unistd.h>
#include <unordered_map>
#include <utility>
#include <vector>

#include "common.h"

#define BSIZE 65536
#define QUEUE_LENGTH 5
#define DISCOVER_ADDR "255.255.255.255"
#define UI_PORT "10277"
#define NAME 0
#define ADDRESS 1
#define PORT 2
#define TIMEOUT (-1)
#define KEY_BUFF_SIZE 3
#define SMALL_SIZE 1

using std::regex;
using std::string;
using std::vector;

using Buffer = std::unordered_map<uint64_t, std::vector<byte>>;
using Sender = std::tuple<string, string, string>;
using SenderTime = std::map<Sender, size_t>;

size_t receive_datagram(int socket_fd, Audio_datagram *datagram,
                        sockaddr_in &address) {
    size_t rec_bytes =
            read_message(socket_fd, &address, datagram, sizeof(Audio_datagram));
    datagram->session_id = be64toh(datagram->session_id);
    datagram->first_byte_num = be64toh(datagram->first_byte_num);

    return rec_bytes;
}

class BufferContainer {
    uint64_t lowest_byte_num{};
    uint64_t highest_byte_num{};
    size_t bsize{};
    size_t psize{};
    size_t max_size{};
    Buffer buffer;
    std::mutex mutex;
    std::condition_variable wait_empty;
    uint64_t start_treshold{};
    bool started = false;
    bool reset = false;
    sockaddr_in rexmit_address;
    int socket_fd = open_socket();

    size_t get_required_size(size_t byte_num) const {
        return (byte_num - lowest_byte_num + psize) / psize;
    }

public:
    explicit BufferContainer(size_t bsize) : bsize(bsize) {
    }

    void put_to_buffer(Audio_datagram &datagram, sockaddr_in &reply_address) {
        std::unique_lock buffer_lock(mutex);
        rexmit_address = reply_address;

        if (datagram.first_byte_num < lowest_byte_num) {
            return;
        }

        if (get_required_size(datagram.first_byte_num) > max_size || reset) {
            buffer.clear();
            lowest_byte_num = datagram.first_byte_num;
            highest_byte_num = datagram.first_byte_num;
            start_treshold = datagram.first_byte_num + (3 * bsize) / 4;
            started = false;
            reset = false;
        }

        buffer[datagram.first_byte_num] =
                vector(datagram.audio_data, datagram.audio_data + psize);
        highest_byte_num = std::max(highest_byte_num, datagram.first_byte_num);
        if (datagram.first_byte_num >= start_treshold) {
            started = true;
            wait_empty.notify_all();
        }
    }

    vector<byte> get_from_buffer() {
        std::unique_lock buffer_lock(mutex);
        wait_empty.wait(buffer_lock, [&] {
            if (!buffer.contains(lowest_byte_num)) {
                reset = true;
            }

            return !reset && started;
        });
        vector to_return = buffer[lowest_byte_num];
        buffer.erase(lowest_byte_num);
        lowest_byte_num += psize;

        return to_return;
    }

    void reset_container(size_t new_psize, Audio_datagram &datagram,
                         sockaddr_in &reply_address) {
        std::unique_lock buffer_lock(mutex);
        rexmit_address = reply_address;
        psize = new_psize;
        max_size = bsize / psize;
        buffer.clear();
        lowest_byte_num = datagram.first_byte_num;
        highest_byte_num = datagram.first_byte_num;
        start_treshold = datagram.first_byte_num + (3 * bsize) / 4;
        buffer[datagram.first_byte_num] =
                vector(datagram.audio_data, datagram.audio_data + psize);
        started = false;
    }

    void send_missing() {
        std::unique_lock buffer_lock(mutex);
        wait_empty.wait(buffer_lock, [&] { return !reset && started; });
        vector<uint64_t> nums;
        for (uint64_t current_byte_num = lowest_byte_num;
             current_byte_num < highest_byte_num; current_byte_num += psize) {
            if (!buffer.contains(current_byte_num)) {
                nums.push_back(current_byte_num);
            }
        }
        sockaddr_in addr = rexmit_address;
        buffer_lock.unlock();

        string rexmit_message = "LOUDER_PLEASE ";
        if (!nums.empty()) {
            rexmit_message += std::to_string(nums[0]);
            for (size_t i = 1; i < nums.size(); i++) {
                rexmit_message += ",";
                rexmit_message += std::to_string(nums[0]);
            }
            rexmit_message += "\n";
            send_message_addr(socket_fd, &addr, rexmit_message.data(),
                              rexmit_message.size());
        }
    }

    size_t get_psize() const {
        return psize;
    }
};

void start_broadcast(BufferContainer &container) {
    vector<byte> to_send;
    while (true) {
        to_send = container.get_from_buffer();
        safe_write(STDOUT_FILENO, to_send.data(), to_send.size());
    }
}

void handle_missing(size_t rtime, BufferContainer &container) {
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(rtime));
        container.send_missing();
    }
}

void read_audio_data(int change_sender, size_t bsize, uint16_t network_port,
                     size_t rtime) {
    Audio_datagram datagram;
    BufferContainer container(bsize);
    size_t rec_bytes;
    size_t change_bytes;
    size_t psize;
    uint64_t session_id = 0;
    bool first_time = true;
    ip_mreq mreq{};
    bool drop = false;
    sockaddr_in bind_address{};
    sockaddr_in send_address{};
    pollfd poll_descriptors[2];
    poll_descriptors[0].fd = open_socket();
    poll_descriptors[1].fd = change_sender;

    for (auto &poll_descriptor: poll_descriptors) {
        poll_descriptor.events = POLLIN;
        poll_descriptor.revents = 0;
    }
    std::thread t(start_broadcast, std::ref(container));
    std::thread t2(handle_missing, rtime, std::ref(container));

    while (true) {
        for (auto &poll_descriptor: poll_descriptors) {
            poll_descriptor.revents = 0;
        }

        CHECK_ERRNO(poll(poll_descriptors, 2, -1));

        if (poll_descriptors[1].revents & POLLIN) {
            if (drop) {
                CHECK_ERRNO(close(poll_descriptors[0].fd));
                poll_descriptors[0].fd = open_socket();
                session_id = 0;
                first_time = true;
            }
            change_bytes = safe_read(change_sender, &bind_address,
                                     sizeof bind_address);
            if (change_bytes == sizeof bind_address) {
                mreq.imr_interface.s_addr = htonl(INADDR_ANY);
                mreq.imr_multiaddr = bind_address.sin_addr;
                CHECK_ERRNO(setsockopt(poll_descriptors[0].fd, IPPROTO_IP,
                                       IP_ADD_MEMBERSHIP, (void *) &mreq,
                                       sizeof(ip_mreq)));
                CHECK_ERRNO(bind(poll_descriptors[0].fd,
                                 (struct sockaddr *) &bind_address,
                                 sizeof bind_address));
                drop = true;
            } else {
                drop = false;
            }
            continue;
        }

        if (poll_descriptors[0].revents & POLLIN) {
            rec_bytes = receive_datagram(poll_descriptors[0].fd, &datagram,
                                         send_address);
            send_address.sin_port = network_port;

            if (datagram.session_id < session_id ||
                rec_bytes <= NUMBERS_FIELD_SIZE ||
                get_psize(rec_bytes) > bsize) {
                continue;
            }

            psize = get_psize(rec_bytes);
            if (datagram.first_byte_num % psize == 0) {
                if (datagram.session_id > session_id || first_time) {
                    first_time = false;
                    session_id = datagram.session_id;
                    container.reset_container(psize, datagram, send_address);
                } else if (psize == container.get_psize()) {
                    container.put_to_buffer(datagram, send_address);
                }
            }
        }
    }
}

void discover_senders(int socket_fd, sockaddr_in &send_address) {
    static const string lookup_mess = "ZERO_SEVEN_COME_IN\n";

    while (true) {
        send_message_addr(socket_fd, &send_address, lookup_mess.data(),
                          lookup_mess.length());
        sleep(5);
    }
}

class SenderList {
    const regex sender_regex =
            regex(R"(^BOREWICZ_HERE ((?:\d|\.)+) ((?:\d)+) (.+)\n$)");
    const string ui_header = "-------------------------------------------------"
                             "-----------------------\n"
                             " SIK Radio\n"
                             "-------------------------------------------------"
                             "-----------------------\n";
    const string ui_footer = "-------------------------------------------------"
                             "-----------------------\n";
    SenderTime senders;
    int change_sender_fd;
    int change_ui_fd;
    string favourite_station;
    Sender playing_sender;
    bool playing = false;
    std::mutex ui_mutex;
    ssize_t change = 0;

private:
    byte reload_buff[SMALL_SIZE];

    string generate_ui() {
        string ui = ui_header;
        for (auto &sender: senders) {
            if (playing && sender.first == playing_sender) {
                ui += " > ";
            }
            ui += get<NAME>(sender.first);
            ui += "\n";
        }
        ui += ui_footer;
        return ui;
    }

    void reload_ui() {
        safe_write(change_ui_fd, reload_buff, SMALL_SIZE);
    }

    void change_station(const Sender &sender) {
        if (playing_sender == sender) {
            return;
        }

        playing_sender = sender;
        sockaddr_in to_send = get_address((get<ADDRESS>(sender)).data(),
                                          (get<PORT>(sender)).data());
        safe_write(change_sender_fd, &to_send, sizeof to_send);
        playing = true;
    }

    void drop_station() {
        Sender new_sender;
        playing_sender = new_sender;
        bool found = false;
        if (favourite_station.empty() && !senders.empty()) {
            new_sender = senders.begin()->first;
            found = true;
        } else {
            for (auto &sender: senders) {
                if (get<NAME>(sender.first) == favourite_station) {
                    new_sender = sender.first;
                    found = true;
                    break;
                }
            }
        }

        if (found) {
            change_station(new_sender);
        } else {
            char drop[] = "D";
            safe_write(change_sender_fd, drop, sizeof drop);
            playing = false;
        }
    }

public:
    explicit SenderList(int pipe1, int pipe2, string name)
        : change_sender_fd(pipe1), change_ui_fd(pipe2),
          favourite_station(std::move(name)) {
    }

    string finish_change() {
        std::unique_lock lock(ui_mutex);
        if (!senders.empty() && change != 0) {
            Sender new_sender;
            if (!playing) {
                new_sender = senders.begin()->first;
                playing = true;
            } else {
                new_sender = playing_sender;
            }

            auto it = senders.find(new_sender);
            while (change > 0 && it != senders.begin()) {
                it--;
                change--;
            }
            while (change < 0 && it != std::prev(senders.end())) {
                it++;
                change++;
            }

            new_sender = it->first;
            change_station(new_sender);
        }
        change = 0;
        return generate_ui();
    }

    void up() {
        change++;
    }

    void down() {
        change--;
    }

    void add(char *message) {
        string addr;
        string port;
        string name;
        std::cmatch match;
        if (std::regex_match(message, match, sender_regex)) {
            addr = string(match[1].str());
            port = string(match[2].str());
            name = string(match[3].str());
            if (is_valid_multicast_addr(addr) && is_port_valid(port.data()) &&
                is_name_valid(name)) {
                Sender sender = std::make_tuple(name, addr, port);
                if (senders.count(sender)) {
                    senders[sender] = time(nullptr);
                } else {
                    senders[sender] = time(nullptr);
                    std::unique_lock lock(ui_mutex);
                    if (!playing && (name == favourite_station ||
                                     favourite_station.empty())) {
                        change_station(sender);
                    }
                    reload_ui();
                }
            }
        }
    }

    uint16_t drop_old(size_t cur_time) {
        bool should_drop = false;
        bool should_reload = false;
        uint16_t min_sleep = 20;
        uint16_t no_response_time;

        for (auto it = senders.begin(); it != senders.end();) {
            no_response_time = cur_time - it->second;
            if (cur_time - it->second >= 20) {
                std::unique_lock lock(ui_mutex);
                should_reload = true;
                if (it->first == playing_sender) {
                    should_drop = true;
                }
                it = senders.erase(it);
                min_sleep = std::min(min_sleep, no_response_time);
            } else {
                it++;
            }
        }

        if (should_drop) {
            std::unique_lock lock(ui_mutex);
            drop_station();
        }

        if (should_reload) {
            std::unique_lock lock(ui_mutex);
            reload_ui();
        }

        return min_sleep;
    }
};

void handle_senders(int socket_fd, SenderList &senderList) {
    vector<pollfd> poll_descriptors;
    uint16_t wait_time;
    pollfd main_fd{socket_fd, POLLIN, 0};
    poll_descriptors.push_back(main_fd);
    char message[MAX_FIELD_SIZE + 1];
    ssize_t message_size;
    socklen_t len = sizeof(sockaddr);

    while (true) {
        errno = EAGAIN;
        message_size =
                recvfrom(socket_fd, message, MAX_FIELD_SIZE, 0, nullptr, &len);
        if (errno != EAGAIN) {
            PRINT_ERRNO();
        }
        message[message_size] = '\0';

        wait_time = senderList.drop_old(time(nullptr));
        if (message_size > 0) {
            senderList.add(message);
        }

        timeval tv{wait_time};
        CHECK_ERRNO(setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &tv,
                               sizeof(tv)));
    }
}

void handle_ui(int socket_fd, int ui_fd, SenderList &senderList) {
    static const string disable_enter = "\xFF\xFD\x22";
    static const string clear_screen = "\033[2J\033[H";
    static const string arrow_up = "\033[A";
    static const string arrow_down = "\033[B";
    string to_send;
    char key_buf[KEY_BUFF_SIZE + 1];
    byte reload_buf[SMALL_SIZE];
    vector<pollfd> poll_descriptors;
    poll_descriptors.push_back(pollfd{socket_fd, POLLIN, 0});
    poll_descriptors.push_back(pollfd{ui_fd, POLLIN, 0});
    CHECK_ERRNO(listen(socket_fd, QUEUE_LENGTH));

    while (true) {
        for (auto &poll_desc: poll_descriptors) {
            poll_desc.revents = 0;
        }

        CHECK_ERRNO(poll(poll_descriptors.data(), poll_descriptors.size(),
                         TIMEOUT));

        if (poll_descriptors[0].revents & POLLIN) {
            int client_fd = accept_connection(poll_descriptors[0].fd, nullptr);
            poll_descriptors.push_back(pollfd{client_fd, POLLIN, 0});
            safe_write(client_fd, disable_enter.data(), disable_enter.size());
            safe_write(client_fd, clear_screen.data(), clear_screen.size());
        }

        if (poll_descriptors[1].revents & POLLIN) {
            safe_read(poll_descriptors[1].fd, reload_buf, SMALL_SIZE);
        }

        for (auto it = std::next(poll_descriptors.begin(), 2);
             it != poll_descriptors.end();) {
            if (it->revents & POLLIN) {
                size_t received_bytes =
                        safe_read(it->fd, key_buf, KEY_BUFF_SIZE);
                key_buf[KEY_BUFF_SIZE] = '\0';
                if (received_bytes == 0) {
                    CHECK_ERRNO(it->fd);
                    it = poll_descriptors.erase(it);
                    continue;
                } else if (received_bytes == KEY_BUFF_SIZE) {
                    string holder(key_buf);
                    if (holder == arrow_up) {
                        senderList.up();
                    } else if (holder == arrow_down) {
                        senderList.down();
                    }
                }
            }
            it++;
        }

        to_send = senderList.finish_change();
        for (auto it = std::next(poll_descriptors.begin(), 2);
             it != poll_descriptors.end(); it++) {
            safe_write(it->fd, clear_screen.data(), clear_screen.size());
            safe_write(it->fd, to_send.data(), to_send.size());
        }
    }
}

int main(int argc, char **argv) {
    int opt;
    opterr = 0;
    char *end = nullptr;

    string ctrl_port = CTRL_PORT;
    string ui_port = UI_PORT;
    string name;
    string discover_addr = DISCOVER_ADDR;
    size_t bsize = BSIZE;
    size_t rtime = RTIME;


    while ((opt = getopt(argc, argv, "b:d:C:U:R:n:")) != -1) {
        switch (opt) {
            case 'b':
                bsize = get_positive_number(optarg, &end);
                if (*end != '\0' || bsize <= 0) {
                    fatal("Wrong bsize.");
                }
                break;
            case 'd':
                discover_addr = optarg;
                if (!is_valid_addr(discover_addr)) {
                    fatal("Not valid discover addr.");
                }
                break;
            case 'C':
                ctrl_port = optarg;
                if (!is_port_valid(ctrl_port.data())) {
                    fatal("Wrong control port.");
                }
                break;
            case 'U':
                ui_port = optarg;
                if (!is_port_valid(ui_port.data())) {
                    fatal("Wrong ui port.");
                }
                break;
            case 'R':
                rtime = get_positive_number(optarg, &end);
                if (rtime <= 0 || *end != '\0') {
                    fatal("Wrong rtime.");
                }
                break;
            case 'n':
                name = optarg;
                if (!is_name_valid(name)) {
                    fatal("Wrong name.");
                }
                break;
            default:
                fatal("Wrong arguments.");
        }
    }

    if (optind < argc) {
        fatal("Wrong arguments.");
    }

    sockaddr_in discover_address =
            get_address(discover_addr.data(), ctrl_port.data());
    int discover_socket = open_socket();
    int optval = 1;
    CHECK_ERRNO(setsockopt(discover_socket, SOL_SOCKET, SO_BROADCAST, &optval,
                           sizeof optval));

    int change_sender[2];
    CHECK_ERRNO(pipe(change_sender));
    int reload_ui[2];
    CHECK_ERRNO(pipe(reload_ui));
    SenderList senderList(change_sender[1], reload_ui[1], name);

    std::thread t(discover_senders, discover_socket,
                  std::ref(discover_address));
    std::thread t2(handle_senders, discover_socket, std::ref(senderList));

    int tcp_socket_fd = open_socket_tcp();
    bind_socket(tcp_socket_fd, std::stoul(ui_port));
    std::thread t3(handle_ui, tcp_socket_fd, reload_ui[0],
                   std::ref(senderList));

    read_audio_data(change_sender[0], bsize, htons(std::stoul(ui_port)), rtime);

    t.join();
    CHECK_ERRNO(close(discover_socket));

    return 0;
}
