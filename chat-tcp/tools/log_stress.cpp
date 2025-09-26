#include <thread>
#include <vector>
#include <atomic>
#include <iostream>
#include "common/logging.hpp"

int main(int argc, char** argv) {
    int n_threads = (argc > 1) ? std::stoi(argv[1]) : 8;
    int n_msgs    = (argc > 2) ? std::stoi(argv[2]) : 1000;

    std::cout << "Rodando stress test: " << n_threads
              << " threads, " << n_msgs << " mensagens cada\n";

    log::L().info("Iniciando stress: {} threads, {} msgs", n_threads, n_msgs);

    std::atomic<int> counter{0};
    std::vector<std::thread> th;
    th.reserve(n_threads);

    for (int t = 0; t < n_threads; ++t) {
        th.emplace_back([t, n_msgs, &counter](){
            for (int i = 0; i < n_msgs; ++i) {
                log::L().debug("[T{}] msg #{}", t, i);
                ++counter;
            }
            log::L().info("[T{}] done", t);
        });
    }

    for (auto& x : th) x.join();

    log::L().info("Total de mensagens: {}", counter.load());
    log::L().info("OK: logging concorrente funcionando.");
    return 0;
}
