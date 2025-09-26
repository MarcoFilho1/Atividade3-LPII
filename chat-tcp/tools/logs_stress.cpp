#include <thread>
#include <vector>
#include <atomic>
#include <iostream>
#include "src/common/logging.hpp"

int main(int argc, char** argv) {
    int n_threads = (argc > 1) ? std::stoi(argv[1]) : 4;
    int n_msgs    = (argc > 2) ? std::stoi(argv[2]) : 100;

    std::cout << "Rodando stress test: " << n_threads
              << " threads, " << n_msgs << " mensagens cada\n";

    std::atomic<int> counter{0};
    std::vector<std::thread> threads;
    threads.reserve(n_threads);

    for (int t = 0; t < n_threads; ++t) {
        threads.emplace_back([t, n_msgs, &counter](){
            for (int i = 0; i < n_msgs; ++i) {
                log::L().info("[Thread {}] mensagem {}", t, i);
                counter++;
            }
        });
    }
    for (auto& th : threads) th.join();

    log::L().info("Stress finalizado. Total de msgs: {}", counter.load());
    return 0;
}
