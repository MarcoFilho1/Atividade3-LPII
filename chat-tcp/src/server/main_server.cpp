#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <string>
#include <iostream>
#include <unistd.h>
#include "common/net.hpp"
#include "common/logging.hpp"

struct Client {
    int fd;
};

static std::string sanitize(const std::string& s) {
    std::string out = s;
    for (char& c : out) { if (c == '\r') c = ' '; }
    return out;
}

int main(int argc, char** argv) {
    uint16_t port = (argc > 1) ? (uint16_t)std::stoi(argv[1]) : 5555;
    int listen_fd = make_server_socket(port);
    if (listen_fd < 0) {
        std::cerr << "Erro ao abrir porta " << port << "\n";
        return 1;
    }

    log::L().info("Servidor ouvindo na porta {}", port);

    std::vector<Client> clients;
    std::mutex clients_mtx;
    std::atomic<bool> running{true};

    // Thread de aceitação
    std::thread accept_th([&](){
        while (running.load()) {
            sockaddr_in cli{}; socklen_t cl = sizeof(cli);
            int cfd = ::accept(listen_fd, (sockaddr*)&cli, &cl);
            if (cfd < 0) { continue; }

            {
                std::lock_guard<std::mutex> lk(clients_mtx);
                clients.push_back(Client{cfd});
            }
            log::L().info("Novo cliente conectado (fd={})", cfd);

            // Thread por cliente
            std::thread([&clients, &clients_mtx, cfd, &running](){
                char buf[1024];
                while (running.load()) {
                    ssize_t n = ::recv(cfd, buf, sizeof(buf), 0);
                    if (n <= 0) break; // desconectou
                    std::string msg(buf, buf + n);
                    auto sp = sanitize(msg);
                    log::L().info("RX fd={} bytes={} msg='{}'", cfd, n, sp);

                    // Prefixo simples com o fd do remetente
                    std::string out = "[fd " + std::to_string(cfd) + "] " + msg;

                    // broadcast 
                    std::lock_guard<std::mutex> lk(clients_mtx);
                    for (auto it = clients.begin(); it != clients.end();) {
                        int tfd = it->fd;
                        if (send_all(tfd, out.data(), out.size()) <= 0) {
                            log::L().warn("Removendo cliente fd={} (send falhou)", tfd);
                            ::close(tfd);
                            it = clients.erase(it);
                        } else {
                            ++it;
                        }
                    }
                }
                log::L().info("Cliente fd={} desconectou", cfd);
                ::close(cfd);
                // remove da lista
                std::lock_guard<std::mutex> lk(clients_mtx);
                for (auto it = clients.begin(); it != clients.end(); ++it) {
                    if (it->fd == cfd) { clients.erase(it); break; }
                }
            }).detach();
        }
    });

    std::cout << "Servidor rodando. Abra 2+ clientes e envie mensagens.\n";
    accept_th.join();
    ::close(listen_fd);
    return 0;
}
