#include <thread>
#include <iostream>
#include <string>
#include <unistd.h>
#include "common/net.hpp"
#include "common/logging.hpp"

int main(int argc, char** argv) {
    std::string host = (argc > 1) ? argv[1] : "127.0.0.1";
    uint16_t    port = (argc > 2) ? (uint16_t)std::stoi(argv[2]) : 5555;

    int fd = connect_to(host, port);
    if (fd < 0) {
        std::cerr << "Falha ao conectar em " << host << ":" << port << "\n";
        return 1;
    }
    log::L().info("Conectado a {}:{}", host, port);

    // Thread receptora
    std::thread rx([&](){
        char buf[1024];
        while (true) {
            ssize_t n = ::recv(fd, buf, sizeof(buf), 0);
            if (n <= 0) break;
            std::cout.write(buf, n);
            std::cout.flush();
        }
        log::L().warn("Conexão encerrada pelo servidor");
    });

    // Envio (stdin -> socket)
    std::string line;
    while (std::getline(std::cin, line)) {
        line.push_back('\n');
        if (send_all(fd, line.data(), line.size()) <= 0) break;
    }

    ::shutdown(fd, SHUT_WR);
    rx.join();
    ::close(fd);
    return 0;
}
