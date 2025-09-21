#include <cstdlib>
#include <string>
#include <thread>
#include <vector>
#include <chrono>
#include <iostream>
#include "tslog/tslog.hpp"

using namespace std::chrono_literals;

static void worker_fn(int id, int msgs) {
    for (int i = 0; i < msgs; ++i) {
        TSLOG_INFO("[worker=", id, "] msg ", i);
        std::this_thread::sleep_for(1ms);
    }
}

int main(int argc, char** argv) {
    int threads = 8;
    int msgs_per_thread = 1000;
    std::string logfile = "logs/app.log";

    if (argc > 1) threads = std::atoi(argv[1]);
    if (argc > 2) msgs_per_thread = std::atoi(argv[2]);
    if (argc > 3) logfile = argv[3];

    auto& L = tslog::Logger::instance();
    L.set_level(tslog::Level::trace);
    L.add_sink(std::make_shared<tslog::ConsoleSink>());
    L.add_sink(std::make_shared<tslog::FileSink>(logfile, 5 * 1024 * 1024, 3));

    TSLOG_INFO("log_stress starting with ", threads, " threads x ", msgs_per_thread, " msgs");

    std::vector<std::jthread> pool;
    pool.reserve(threads);
    for (int t = 0; t < threads; ++t) pool.emplace_back(worker_fn, t, msgs_per_thread);

    TSLOG_INFO("waiting workers...");
    TSLOG_INFO("done");
    return 0;
}
