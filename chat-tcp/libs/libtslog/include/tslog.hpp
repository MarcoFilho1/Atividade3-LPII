#pragma once
#include <mutex>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <sstream>
#include <chrono>
#include <ctime>
#include <filesystem>

namespace tslog {

enum class Level { Debug=0, Info=1, Warn=2, Error=3 };

inline const char* level_name(Level lv) {
    switch (lv) {
        case Level::Debug: return "DEBUG";
        case Level::Info:  return "INFO ";
        case Level::Warn:  return "WARN ";
        default:           return "ERROR";
    }
}

inline std::string now_str() {
    using namespace std::chrono;
    auto now = system_clock::now();
    std::time_t t = system_clock::to_time_t(now);
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&t));
    return std::string(buf);
}

class Logger {
public:
    Level       min_level = Level::Info;
    bool        to_stdout = true;
    std::string file_path = "logs/app.log";

    Logger() { open_file(); }
    Logger(Level min, bool out, std::string path)
        : min_level(min), to_stdout(out), file_path(std::move(path)) { open_file(); }

    // API com placeholders "{}"
    template<typename... Args>
    void debug(std::string_view fmt, Args&&... args) { log(Level::Debug, format(fmt, std::forward<Args>(args)...)); }
    template<typename... Args>
    void info (std::string_view fmt, Args&&... args) { log(Level::Info,  format(fmt, std::forward<Args>(args)...)); }
    template<typename... Args>
    void warn (std::string_view fmt, Args&&... args) { log(Level::Warn,  format(fmt, std::forward<Args>(args)...)); }
    template<typename... Args>
    void error(std::string_view fmt, Args&&... args) { log(Level::Error, format(fmt, std::forward<Args>(args)...)); }

    // Overloads sem formatação
    void debug(std::string_view s) { log(Level::Debug, std::string(s)); }
    void info (std::string_view s) { log(Level::Info,  std::string(s)); }
    void warn (std::string_view s) { log(Level::Warn,  std::string(s)); }
    void error(std::string_view s) { log(Level::Error, std::string(s)); }

private:
    std::mutex   m_;
    std::ofstream fout_;

    void open_file() {
        try {
            auto p = std::filesystem::path(file_path).parent_path();
            if (!p.empty()) std::filesystem::create_directories(p);
        } catch (...) { /* fallback: apenas stdout */ }
        fout_.open(file_path, std::ios::app);
    }

    void log(Level lv, const std::string& msg) {
        if (lv < min_level) return;
        std::lock_guard<std::mutex> lk(m_);
        const std::string line = now_str() + " [" + level_name(lv) + "] " + msg + "\n";
        if (to_stdout) { std::cout << line; std::cout.flush(); }
        if (fout_.is_open()) { fout_ << line; fout_.flush(); }
    }

    template<typename T>
    static std::string to_string_any(const T& v) { std::ostringstream os; os << v; return os.str(); }

    static void replace_first(std::string& s, const std::string& what, const std::string& with) {
        if (auto pos = s.find(what); pos != std::string::npos) s.replace(pos, what.size(), with);
    }

    template<typename... Args>
    static std::string format(std::string_view fmt, Args&&... args) {
        std::string s(fmt);
        (replace_first(s, "{}", to_string_any(args)), ...);
        return s;
    }
};

} 
