#pragma once
#include <string>
#include <string_view>
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

inline int make_server_socket(uint16_t port, int backlog = 64) {
    int fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return -1;

    int yes = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (::bind(fd, (sockaddr*)&addr, sizeof(addr)) < 0) { ::close(fd); return -1; }
    if (::listen(fd, backlog) < 0) { ::close(fd); return -1; }
    return fd;
}

inline int connect_to(std::string_view host, uint16_t port) {
    int fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return -1;

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(port);
    if (::inet_pton(AF_INET, std::string(host).c_str(), &addr.sin_addr) <= 0) { ::close(fd); return -1; }

    if (::connect(fd, (sockaddr*)&addr, sizeof(addr)) < 0) { ::close(fd); return -1; }
    return fd;
}

inline ssize_t send_all(int fd, const void* buf, size_t len) {
    const char* p = static_cast<const char*>(buf);
    size_t left = len;
    while (left) {
        ssize_t n = ::send(fd, p, left, 0);
        if (n <= 0) return n;
        p += n; left -= n;
    }
    return (ssize_t)len;
}
