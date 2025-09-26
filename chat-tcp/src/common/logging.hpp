#pragma once
#include <tslog.hpp>

namespace log {
    inline tslog::Logger& L() {
        static tslog::Logger logger{
            .min_level = tslog::Level::Debug,
            .to_stdout = true,
            .file_path = "logs/app.log"
        };
        return logger;
    }
}
