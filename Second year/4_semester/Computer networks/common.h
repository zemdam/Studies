#ifndef MIMUW_SIK_TCP_SOCKETS_COMMON_H
#define MIMUW_SIK_TCP_SOCKETS_COMMON_H

#include <arpa/inet.h>
#include <netdb.h>
#include <csignal>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "err.h"

#define NO_FLAGS 0
#define DATA_PORT "20277"
#define MAX_FIELD_SIZE 65527
#define TOO_BIG_SIZE 65528
#define NUMBERS_FIELD_SIZE 16
#define MAX_DATA (MAX_FIELD_SIZE - NUMBERS_FIELD_SIZE)
#define CTRL_PORT "30277"
#define RTIME 250

typedef uint8_t byte;

typedef struct __attribute__ ((__packed__)) {
    uint64_t session_id;
    uint64_t first_byte_num;
    byte audio_data[MAX_DATA];
} Audio_datagram;

void *safe_malloc(size_t size) {
    void *memory = malloc(size);

    if (memory == nullptr) {
        fatal("Malloc failed.");
    }

    return memory;
}

size_t audio_datagram_size(size_t psize) {
    return NUMBERS_FIELD_SIZE + psize;
}

size_t get_psize(size_t datagram_size) {
    return datagram_size - NUMBERS_FIELD_SIZE;
}

uint16_t is_port_valid(char *string) {
    errno = 0;
    char *end = nullptr;
    unsigned long port = strtoul(string, &end, 10);
    PRINT_ERRNO();
    if (port > UINT16_MAX || *end != '\0') {
        return false;
    }

    return true;
}

struct sockaddr_in get_address(char const *host, char const *port) {
    struct addrinfo hints{};
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    struct addrinfo *address_result;
    CHECK(getaddrinfo(host, port, &hints, &address_result));

    struct sockaddr_in send_address{};
    send_address.sin_family = AF_INET; // IPv4
    send_address.sin_addr.s_addr =
            ((struct sockaddr_in *) (address_result->ai_addr))
                    ->sin_addr.s_addr;   // IP address
    send_address.sin_port = ((struct sockaddr_in *) (address_result->ai_addr))->sin_port; // port from the command line

    freeaddrinfo(address_result);

    return send_address;
}

int open_socket() {
    int socket_fd = socket(PF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        PRINT_ERRNO();
    }

    return socket_fd;
}

inline static int open_socket_tcp() {
    int socket_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socket_fd < 0) {
        PRINT_ERRNO();
    }

    return socket_fd;
}

void bind_socket(int socket_fd, uint16_t port) {
    struct sockaddr_in server_address{};
    server_address.sin_family = AF_INET; // IPv4
    server_address.sin_addr.s_addr =
            htonl(INADDR_ANY); // listening on all interfaces
    server_address.sin_port = htons(port);

    // bind the socket to a concrete address
    CHECK_ERRNO(bind(socket_fd, (struct sockaddr *) &server_address,
                     (socklen_t) sizeof(server_address)));
}

void connect_socket(int socket_fd,
                                  const struct sockaddr_in *address) {
    CHECK_ERRNO(
            connect(socket_fd, (struct sockaddr *) address, sizeof(*address)));
}

void send_message(int socket_fd, const void *message,
                                size_t length, int flags) {
    errno = 0;
    ssize_t sent_length = send(socket_fd, message, length, flags);

    if (sent_length < 0) {
        PRINT_ERRNO();
    }
    ENSURE(sent_length == (ssize_t) length);
}

size_t receive_message(int socket_fd, void *buffer,
                                     size_t max_length, int flags) {
    errno = 0;
    ssize_t received_length = recv(socket_fd, buffer, max_length, flags);
    if (received_length < 0) {
        PRINT_ERRNO();
    }
    return (size_t) received_length;
}

size_t read_message(int socket_fd, struct sockaddr_in *client_address, void *buffer, size_t max_length) {
    socklen_t address_length = (socklen_t) sizeof(*client_address);
    int flags = 0; // we do not request anything special
    errno = 0;
    ssize_t len = recvfrom(socket_fd, buffer, max_length, flags,
                           (struct sockaddr *) client_address, &address_length);
    if (len < 0) {
        PRINT_ERRNO();
    }
    return (size_t) len;
}

void send_message_addr(int socket_fd, const struct sockaddr_in *send_address, const void *message, size_t message_length) {
    int send_flags = NO_FLAGS;
    socklen_t address_length = (socklen_t) sizeof(*send_address);
    errno = 0;
    ssize_t sent_length = sendto(socket_fd, message, message_length, send_flags,
                                 (struct sockaddr *) send_address, address_length);
    if (sent_length < 0) {
        PRINT_ERRNO();
    }
    ENSURE(sent_length == (ssize_t) message_length);
}

void safe_write(int socket_fd, const void *message, size_t size) {
    ssize_t sent_length = write(socket_fd, message, size);
    if (sent_length < 0) {
        PRINT_ERRNO();
    }
    ENSURE(sent_length == (ssize_t) size);
}

size_t safe_read(int socket_fd, void *message, size_t size) {
    ssize_t sent_length = read(socket_fd, message, size);
    if (sent_length < 0) {
        PRINT_ERRNO();
    }
    return sent_length;
}

size_t get_positive_number(char *string, char **end) {
    if (string[0] == '-') {
        fatal("Expected positive number");
    }

    return strtoull(string, end, 10);
}

bool is_valid_addr(std::string &addr) {
    struct sockaddr_in multicast_addr{};
    int result = inet_pton(AF_INET, addr.data(), &(multicast_addr.sin_addr));

    return result == 1;
}

bool is_name_valid(std::string &name) {
    size_t i;
    for (i = 0; name[i] != '\0'; i++) {
        if (name[i] < 32 || name[i] >= 127) {
            return false;
        }
    }

    if (name[0] == '\0' || name[0] == ' ' || name[i-1] == ' ' || name.length() > 64) {
        return false;
    }

    return true;
}

bool is_valid_multicast_addr(std::string &addr) {
    struct sockaddr_in multicast_addr{};
    int result = inet_pton(AF_INET, addr.data(), &(multicast_addr.sin_addr));

    return result == 1 && IN_MULTICAST(ntohl(multicast_addr.sin_addr.s_addr));
}

int accept_connection(int socket_fd, struct sockaddr_in *client_address) {
    socklen_t client_address_length = (socklen_t) sizeof(*client_address);

    int client_fd = accept(socket_fd, (struct sockaddr *) client_address, &client_address_length);
    if (client_fd < 0) {
        PRINT_ERRNO();
    }

    return client_fd;
}

#endif //MIMUW_SIK_TCP_SOCKETS_COMMON_H
