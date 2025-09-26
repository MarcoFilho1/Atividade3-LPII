#pragma once
#include <tslog.hpp>

namespace log {
// Singleton thread-safe do logger
inline tslog::Logger& L() {
    static tslog::Logger logger{
        tslog::Level::Debug,   // nível mínimo
        true,                  // stdout habilitado
        "logs/app.log"         // arquivo de saída
    };
    return logger;
}
} 
