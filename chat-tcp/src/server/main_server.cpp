#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <string>
#include <csignal>
#include <iostream>
#include <unistd.h>
#include <cerrno>

#include "common/net.hpp"
#include "common/logging.hpp"
#include "server/ThreadSafeQueue.hpp"

struct Client { int fd; };

// --------- Controle de execução (SIGINT) ----------
static std::atomic_bool running{true};
static void sigint_handler(int) { running.store(false); }

// Remove '\r' do fim da linha
static void trim_cr(std::string& s) {
    while (!s.empty() && s.back() == '\r') s.pop_back();
}

int main(int argc, char** argv) {
    std::signal(SIGINT, sigint_handler);

    // Uso: ./chat_server [porta]
    if (argc > 2) {
        std::cerr << "Uso: " << argv[0] << " [porta]\nEx.: " << argv[0] << " 5555\n";
        return 2;
    }
    uint16_t port = (argc > 1) ? static_cast<uint16_t>(std::stoi(argv[1])) : 5555;

    int listen_fd = make_server_socket(port);
    if (listen_fd < 0) {
        std::cerr << "Erro ao abrir porta " << port << "\n";
        return 1;
    }
    log::L().info("Servidor ouvindo na porta {}", port);
    std::cout << "Servidor rodando (Ctrl+C para encerrar)\n";

    // --------- Estruturas compartilhadas ----------
    std::vector<Client> clients;
    std::mutex clients_mtx;

    // Histórico simples
    std::vector<std::string> history;
    const size_t HISTORY_MAX = 200;
    std::mutex history_mtx;

    // Fila bounded (monitor) para mensagens a serem broadcastadas
    ThreadSafeQueue queue(1024);

    // --------- Thread: Broadcaster ----------
    std::thread broadcaster([&](){
        while (true) {
            auto msg_opt = queue.pop(running);
            if (!msg_opt.has_value()) break; // shutdown sem itens
            const std::string& msg = *msg_opt;

            // Grava no histórico
            {
                std::lock_guard<std::mutex> lk(history_mtx);
                history.push_back(msg);
                if (history.size() > HISTORY_MAX) history.erase(history.begin());
            }

            // Envia a TODOS os clientes conectados
            std::lock_guard<std::mutex> lk(clients_mtx);
            for (auto it = clients.begin(); it != clients.end();) {
                if (send_all(it->fd, msg.data(), msg.size()) <= 0) {
                    log::L().warn("Removendo cliente fd={} (send falhou)", it->fd);
                    ::close(it->fd);
                    it = clients.erase(it);
                } else {
                    ++it;
                }
            }
            // Log de amostra (primeiros 80 chars)
            if (!msg.empty()) {
                log::L().debug("Broadcast: {}", msg.substr(0, std::min<size_t>(msg.size(), 80)));
            }
        }
        log::L().info("Broadcaster finalizado");
    });

    // --------- Thread: Aceitação de clientes ----------
    std::thread accept_th([&](){
        while (running.load()) {
            sockaddr_in cli{}; socklen_t cl = sizeof(cli);
            int cfd = ::accept(listen_fd, (sockaddr*)&cli, &cl);
            if (cfd < 0) {
                if (!running.load()) break;
                int e = errno;
                if (e == EBADF || e == EINVAL) { // fd inválido/fechado -> estamos encerrando
                    log::L().info("Aceitação finalizada (listen_fd fechado)");
                    break;
                }
                if (e == EINTR) continue; // sinal -> tenta de novo
                // demais erros transitórios -> continua
                continue;
            }

            {
                std::lock_guard<std::mutex> lk(clients_mtx);
                clients.push_back(Client{cfd});
            }
            log::L().info("Novo cliente conectado (fd={})", cfd);

            // Envia histórico ao novo cliente
            {
                std::lock_guard<std::mutex> lk(history_mtx);
                for (auto& line : history) {
                    if (send_all(cfd, line.data(), line.size()) <= 0) break;
                }
            }

            // Thread por cliente: recebe por linhas e publica na fila
            std::thread([&, cfd](){
                char buf[1024];
                std::string acc;
                while (running.load()) {
                    ssize_t n = ::recv(cfd, buf, sizeof(buf), 0);
                    if (n <= 0) break; // desconectou ou erro
                    acc.append(buf, buf + n);

                    // Processa por linhas
                    for (;;) {
                        auto pos = acc.find('\n');
                        if (pos == std::string::npos) break;
                        std::string line = acc.substr(0, pos);
                        acc.erase(0, pos + 1);
                        trim_cr(line);
                        if (line.empty()) continue;

                        std::string out = line + "\n";

                        if (!queue.push(out, running)) break;
                        log::L().info("RX fd={} '{}'", cfd, line);
                    }
                }
                log::L().info("Cliente fd={} desconectou", cfd);
                ::close(cfd);
                // Remove da lista
                std::lock_guard<std::mutex> lk(clients_mtx);
                for (auto it = clients.begin(); it != clients.end(); ++it) {
                    if (it->fd == cfd) { clients.erase(it); break; }
                }
            }).detach();
        }
        log::L().info("Aceitação finalizada");
    });

    while (running.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // --------- SHUTDOWN ----------
    log::L().info("Encerrando servidor...");
    running.store(false);

    // Acorda broadcaster/consumidores
    queue.notify_all();

    // Desbloqueia accept()
    ::shutdown(listen_fd, SHUT_RDWR);
    ::close(listen_fd);

    // Desbloqueia possíveis recv() nos clientes (threads destacadas)
    {
        std::lock_guard<std::mutex> lk(clients_mtx);
        for (auto& c : clients) {
            ::shutdown(c.fd, SHUT_RDWR);
        }
    }

    // Junta threads longas
    accept_th.join();
    broadcaster.join();

    // Fecha FDs remanescentes (se alguma thread não fechou)
    {
        std::lock_guard<std::mutex> lk(clients_mtx);
        for (auto& c : clients) ::close(c.fd);
        clients.clear();
    }

    log::L().info("Servidor finalizado com sucesso");
    return 0;
}
