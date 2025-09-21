#include "tslog/tslog.hpp"

namespace tslog {

void Logger::worker_loop() {
    while (run_.load(std::memory_order_relaxed)) {
        LogRecord rec;
        if (!queue_.pop(rec)) continue;
        if (!run_.load(std::memory_order_relaxed) && rec.level == Level::off) break;
        auto line = format_line(rec);
        std::lock_guard<std::mutex> lock(sinks_mtx_);
        for (auto& s : sinks_) s->write(line);
    }
}

} 
