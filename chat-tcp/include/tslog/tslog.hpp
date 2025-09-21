#pragma once
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <semaphore>
#include <sstream>
#include <string>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>

namespace tslog {

// ---------- Níveis ----------
enum class Level : int { trace, debug, info, warn, error, critical, off };
inline std::string_view to_string(Level lvl) {
    switch (lvl) {
        case Level::trace:    return "TRACE";
        case Level::debug:    return "DEBUG";
        case Level::info:     return "INFO";
        case Level::warn:     return "WARN";
        case Level::error:    return "ERROR";
        case Level::critical: return "CRIT";
        case Level::off:      return "OFF";
    }
    return "?";
}

// ---------- Estruturas ----------
struct SourceLoc { std::string file; int line{0}; std::string func; };
struct LogRecord {
    Level level{Level::info};
    std::string message;
    SourceLoc src;
    std::thread::id tid;
    std::chrono::system_clock::time_point ts;
};

// ---------- Sinks ----------
struct Sink { virtual ~Sink() = default; virtual void write(const std::string& line)=0; virtual void flush(){}; };

class ConsoleSink final : public Sink {
public: void write(const std::string& line) override { std::cout << line << '\n'; }
        void flush() override { std::cout.flush(); } };

class FileSink final : public Sink {
public:
    explicit FileSink(std::filesystem::path path, std::size_t rotate_bytes=0, int keep=3)
    : base_path_(std::move(path)), rotate_bytes_(rotate_bytes), keep_(keep) { open_new_if_needed(); }
    void write(const std::string& line) override {
        if (!ofs_.is_open()) open_new_if_needed();
        ofs_ << line << '\n';
        if (rotate_bytes_>0 && ofs_.tellp() >= static_cast<std::streamoff>(rotate_bytes_)) rotate();
    }
    void flush() override { ofs_.flush(); }
private:
    std::filesystem::path base_path_;
    std::ofstream ofs_;
    std::size_t rotate_bytes_{0};
    int keep_{3};
    void open_new_if_needed() {
        std::filesystem::create_directories(base_path_.parent_path());
        ofs_.open(base_path_, std::ios::out | std::ios::app);
    }
    void rotate() {
        ofs_.close();
        for (int i = keep_-1; i >= 1; --i) {
            auto from = base_path_.string() + "." + std::to_string(i);
            auto to   = base_path_.string() + "." + std::to_string(i+1);
            if (std::filesystem::exists(from)) { std::error_code ec; std::filesystem::rename(from, to, ec); }
        }
        auto first = base_path_.string() + ".1";
        std::error_code ec; std::filesystem::rename(base_path_, first, ec);
        open_new_if_needed();
    }
};

// ---------- Monitor: Fila Limitada ----------
template <class T>
class BoundedQueue {
public:
    explicit BoundedQueue(std::size_t capacity)
    : capacity_(capacity), slots_(static_cast<std::ptrdiff_t>(capacity)), items_(0) {}

    void push(T value) {
        slots_.acquire();
        { std::lock_guard<std::mutex> lock(mtx_); q_.push(std::move(value)); }
        items_.release();
    }

    bool pop(T& out) {
        items_.acquire();
        std::lock_guard<std::mutex> lock(mtx_);
        if (q_.empty()) { slots_.release(); return false; }
        out = std::move(q_.front()); q_.pop(); slots_.release(); return true;
    }

    std::size_t size() const { std::lock_guard<std::mutex> lock(mtx_); return q_.size(); }

private:
    std::size_t capacity_;
    mutable std::mutex mtx_;
    std::queue<T> q_;
    std::counting_semaphore<> slots_;
    std::counting_semaphore<> items_;
};

// ---------- Logger ----------
class Logger {
public:
    explicit Logger(std::size_t queue_capacity=8192) : queue_(queue_capacity) {
        run_.store(true); worker_ = std::jthread([this]{ worker_loop(); });
    }
    ~Logger(){ shutdown(); }

    void add_sink(std::shared_ptr<Sink> s){ std::lock_guard<std::mutex> lock(sinks_mtx_); sinks_.push_back(std::move(s)); }
    void set_level(Level lvl){ level_.store(lvl, std::memory_order_relaxed); }
    Level level() const { return level_.load(std::memory_order_relaxed); }

    template <typename... Args>
    void log(Level lvl, SourceLoc src, Args&&... args) {
        if (lvl < level()) return;
        LogRecord rec; rec.level=lvl; rec.message=concat(std::forward<Args>(args)...);
        rec.src=std::move(src); rec.tid=std::this_thread::get_id(); rec.ts=std::chrono::system_clock::now();
        queue_.push(std::move(rec));
    }
    template <typename... A> void trace(SourceLoc s, A&&... a){ log(Level::trace, s, std::forward<A>(a)...); }
    template <typename... A> void debug(SourceLoc s, A&&... a){ log(Level::debug, s, std::forward<A>(a)...); }
    template <typename... A> void info (SourceLoc s, A&&... a){ log(Level::info , s, std::forward<A>(a)...); }
    template <typename... A> void warn (SourceLoc s, A&&... a){ log(Level::warn , s, std::forward<A>(a)...); }
    template <typename... A> void error(SourceLoc s, A&&... a){ log(Level::error, s, std::forward<A>(a)...); }
    template <typename... A> void crit (SourceLoc s, A&&... a){ log(Level::critical, s, std::forward<A>(a)...); }

    void flush(){ std::lock_guard<std::mutex> lock(sinks_mtx_); for (auto& s: sinks_) s->flush(); }

    void shutdown(){
        bool expected=true;
        if (run_.compare_exchange_strong(expected,false)){
            LogRecord poison; poison.level=Level::off; queue_.push(std::move(poison));
            if (worker_.joinable()) worker_.join(); flush();
        }
    }

    static Logger& instance(){ static Logger L; return L; }

private:
    std::atomic<bool> run_{false};
    std::jthread worker_;
    BoundedQueue<LogRecord> queue_;
    std::vector<std::shared_ptr<Sink>> sinks_;
    std::mutex sinks_mtx_;
    std::atomic<Level> level_{Level::trace};

    static std::string format_time(const std::chrono::system_clock::time_point& tp){
        auto t = std::chrono::system_clock::to_time_t(tp); std::tm tm{};
    #if defined(_WIN32)
        localtime_s(&tm, &t);
    #else
        localtime_r(&t, &tm);
    #endif
        char buf[32];
        std::snprintf(buf,sizeof(buf),"%04d-%02d-%02d %02d:%02d:%02d",
                      tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
        return std::string(buf);
    }
    static std::string format_line(const LogRecord& r){
        std::ostringstream os;
        os << '[' << format_time(r.ts) << "] [" << to_string(r.level) << "] [tid=" << r.tid << "] ";
        if (!r.src.file.empty()) os << r.src.file << ':' << r.src.line << ": ";
        os << r.message; return os.str();
    }

    void worker_loop();

    template <typename... Args> static std::string concat(Args&&... args){
        std::ostringstream os; (os << ... << args); return os.str();
    }
};

// Macros de conveniência
#define TSLOG_SRC ::tslog::SourceLoc{__FILE__, __LINE__, __func__}
#define TSLOG_TRACE(...) ::tslog::Logger::instance().trace(TSLOG_SRC, __VA_ARGS__)
#define TSLOG_DEBUG(...) ::tslog::Logger::instance().debug(TSLOG_SRC, __VA_ARGS__)
#define TSLOG_INFO(...)  ::tslog::Logger::instance().info (TSLOG_SRC, __VA_ARGS__)
#define TSLOG_WARN(...)  ::tslog::Logger::instance().warn (TSLOG_SRC, __VA_ARGS__)
#define TSLOG_ERROR(...) ::tslog::Logger::instance().error(TSLOG_SRC, __VA_ARGS__)
#define TSLOG_CRIT(...)  ::tslog::Logger::instance().crit (TSLOG_SRC, __VA_ARGS__)

} // namespace tslog
