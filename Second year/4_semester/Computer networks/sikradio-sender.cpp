#include <arpa/inet.h>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <mutex>
#include <poll.h>
#include <regex>
#include <string>
#include <thread>
#include <unistd.h>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "common.h"

#define PSIZE 512
#define FSIZE 131072
#define RTIME 250
#define NAZWA "Nienazwany Nadajnik"
#define POLLS_NUM 2

using std::regex;
using std::regex_match;
using std::regex_search;
using std::smatch;
using std::string;
using std::unordered_map;
using std::vector;

using FIFO = unordered_map<uint64_t, vector<byte>>;

struct FIFOSync {
    FIFO fifo;
    std::mutex mutex;
    uint64_t session_id{};
    size_t psize{};
};

struct SetSync {
    std::unordered_set<uint64_t> set;
    std::mutex mutex;
    std::condition_variable condition_var;
    bool end = false;
};

void send_audio(int socket_fd, int end_socket, size_t fifo_max_size,
                size_t psize, FIFOSync &fifo_sync, sockaddr_in &send_address) {
    size_t datagram_size = audio_datagram_size(psize);
    std::unique_lock fifo_lock{fifo_sync.mutex, std::defer_lock};
    FIFO &fifo = fifo_sync.fifo;
    Audio_datagram datagram;
    datagram.first_byte_num = 0;
    uint64_t current_byte_num = 0;
    datagram.session_id = fifo_sync.session_id;

    while (fread(datagram.audio_data, psize, 1, stdin) == 1) {
        fifo_lock.lock();
        if (fifo.size() >= fifo_max_size && !fifo.empty()) {
            fifo.erase(current_byte_num - psize * fifo_max_size);
        }

        if (fifo.size() < fifo_max_size) {
            fifo[current_byte_num] = vector<byte>(datagram.audio_data,
                                                  datagram.audio_data + psize);
        }
        fifo_lock.unlock();

        datagram.first_byte_num = htobe64(current_byte_num);
        send_message_addr(socket_fd, &send_address, &datagram, datagram_size);
        current_byte_num += psize;
    }

    safe_write(end_socket, datagram.audio_data, 1);
}

void add_to_set(SetSync &setSync, char const *message, size_t message_size) {
    std::unique_lock list_lock{setSync.mutex, std::defer_lock};
    vector<uint64_t> numbers;
    regex match_number(R"((\d+))");
    smatch number_matches;
    std::string message_str(message);

    list_lock.lock();
    while (regex_search(message_str, number_matches, match_number)) {
        setSync.set.insert(std::stoull(number_matches[1].str()));
        message_str = number_matches.suffix().str();
    }
    list_lock.unlock();
    setSync.condition_var.notify_one();
}

void handle_control(int socket_fd, int end_socket, SetSync &setSync,
                    string &reply_message) {
    size_t message_size;
    sockaddr_in client_address{};
    regex rexmit(R"(^LOUDER_PLEASE ((\d+),)*\d+\n$)");
    regex lookup("^ZERO_SEVEN_COME_IN\n$");
    char message[MAX_FIELD_SIZE + 1];
    pollfd poll_descriptors[POLLS_NUM];
    poll_descriptors[0].fd = socket_fd;
    poll_descriptors[1].fd = end_socket;
    for (auto &poll_descriptor: poll_descriptors) {
        poll_descriptor.events = POLLIN;
        poll_descriptor.revents = 0;
    }

    while (true) {
        for (auto &poll_descriptor: poll_descriptors) {
            poll_descriptor.revents = 0;
        }

        CHECK_ERRNO(poll(poll_descriptors, 2, -1));

        if (poll_descriptors[1].revents & POLLIN) {
            std::unique_lock set_lock(setSync.mutex);
            setSync.end = true;
            setSync.condition_var.notify_one();
            return;
        }

        if (poll_descriptors[0].revents & POLLIN) {
            message_size = read_message(socket_fd, &client_address, message,
                                        MAX_FIELD_SIZE);
            message[message_size] = '\0';

            if (regex_match(message, rexmit)) {
                add_to_set(setSync, message, message_size);
            } else if (regex_match(message, lookup)) {
                send_message_addr(socket_fd, &client_address,
                                  reply_message.data(), reply_message.length());
            }
        }
    }
}

void send_rexmit(int socket_fd, FIFOSync &fifoSync,
                 std::unordered_set<uint64_t> &num_set,
                 sockaddr_in &send_address) {
    Audio_datagram datagram;
    size_t datagram_size = audio_datagram_size(fifoSync.psize);
    datagram.session_id = fifoSync.session_id;
    std::unique_lock fifo_lock(fifoSync.mutex);
    for (uint64_t num: num_set) {
        if (fifoSync.fifo.contains(num)) {
            memcpy(datagram.audio_data, fifoSync.fifo.at(num).data(),
                   fifoSync.psize);
            fifo_lock.unlock();
            datagram.first_byte_num = num;
            send_message_addr(socket_fd, &send_address, &datagram,
                              datagram_size);
            fifo_lock.lock();
        }
    }
}

void handle_rexmit(int socket_fd, int end_socket, SetSync &setSync,
                   FIFOSync &fifoSync, size_t rtime,
                   sockaddr_in &send_address) {
    pollfd poll_descriptors[POLLS_NUM];
    poll_descriptors[0].fd = socket_fd;
    poll_descriptors[0].events = POLLOUT;
    poll_descriptors[1].fd = end_socket;
    poll_descriptors[1].events = POLLIN;
    std::this_thread::sleep_for(std::chrono::milliseconds(rtime));

    while (true) {
        for (auto &poll_descriptor: poll_descriptors) {
            poll_descriptor.revents = 0;
        }

        CHECK_ERRNO(poll(poll_descriptors, 2, -1));

        if (poll_descriptors[1].revents & POLLIN) {
            return;
        }

        if (poll_descriptors[0].revents & POLLOUT) {
            std::unique_lock set_lock(setSync.mutex);
            setSync.condition_var.wait(set_lock, [&] {
                return !setSync.set.empty() || setSync.end;
            });
            if (setSync.end) {
                return;
            }
            std::unordered_set copy_set(setSync.set);
            setSync.set.clear();
            set_lock.unlock();

            send_rexmit(socket_fd, fifoSync, copy_set, send_address);

            std::this_thread::sleep_for(std::chrono::milliseconds(rtime));
        }
    }
}

bool is_udp_size_valid(size_t size) {
    return size > 0 && size <= MAX_DATA;
}

int main(int argc, char **argv) {
    int opt;
    opterr = 0;

    string dest_addr;
    string name = NAZWA;
    size_t psize = PSIZE;
    size_t fsize = FSIZE;
    size_t rtime = RTIME;
    string data_port = DATA_PORT;
    string ctrl_port = CTRL_PORT;
    char *end = nullptr;

    while ((opt = getopt(argc, argv, "a:p:P:n:f:R:C:")) != -1) {
        switch (opt) {
            case 'a':
                dest_addr = optarg;
                if (!is_valid_multicast_addr(dest_addr)) {
                    fatal("Wrong multicast address.");
                }
                break;
            case 'p':
                psize = get_positive_number(optarg, &end);
                if (!is_udp_size_valid(psize) || *end != '\0') {
                    fatal("Wrong psize.");
                }
                break;
            case 'f':
                fsize = get_positive_number(optarg, &end);
                if (*end != '\0') {
                    fatal("Wrong fsize.");
                }
                break;
            case 'R':
                rtime = get_positive_number(optarg, &end);
                if (rtime <= 0 || *end != '\0') {
                    fatal("Wrong rtime.");
                }
                break;
            case 'P':
                data_port = optarg;
                if (!is_port_valid(data_port.data())) {
                    fatal("Wrong data port.");
                }
                break;
            case 'C':
                ctrl_port = optarg;
                if (!is_port_valid(ctrl_port.data())) {
                    fatal("Wrong control port.");
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

    if (dest_addr.empty()) {
        fatal("No destination address.");
    }

    if (data_port == ctrl_port) {
        fatal("Data port and ctrl port are equal.");
    }

    string reply_message =
            "BOREWICZ_HERE " + dest_addr + " " + data_port + " " + name + "\n";

    struct sockaddr_in server_address =
            get_address(dest_addr.data(), data_port.data());
    int end_pipe[2];
    CHECK_ERRNO(pipe(end_pipe));
    int output_socket = open_socket();
    int input_socket = open_socket();
    int optval = 1;
    bind_socket(input_socket, std::stoul(ctrl_port));
    CHECK_ERRNO(setsockopt(input_socket, SOL_SOCKET, SO_BROADCAST, &optval,
                           sizeof optval));
    FIFOSync fifo_sync;
    fifo_sync.session_id = time(nullptr);
    fifo_sync.session_id = htobe64(fifo_sync.session_id);
    fifo_sync.psize = psize;
    SetSync setSync;
    std::thread t(handle_control, input_socket, end_pipe[0], std::ref(setSync),
                  std::ref(reply_message));
    std::thread t2(handle_rexmit, output_socket, end_pipe[0], std::ref(setSync),
                   std::ref(fifo_sync), rtime, std::ref(server_address));
    send_audio(output_socket, end_pipe[1], fsize / psize, psize, fifo_sync,
               server_address);

    t.join();
    t2.join();
    CHECK_ERRNO(close(output_socket));
    CHECK_ERRNO(close(input_socket));
    CHECK_ERRNO(close(end_pipe[0]));
    CHECK_ERRNO(close(end_pipe[1]));

    return 0;
}