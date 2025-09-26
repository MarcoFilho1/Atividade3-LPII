#pragma once
#include <tslog.hpp>

namespace log {
// Singleton thread-safe do logger
inline tslog::Logger& L() {
    static tslog::Logger logger{
        tslog::Level::Debug,   // n�vel m�nimo
        true,                  // stdout habilitado
        "logs/app.log"         // arquivo de sa�da
    };
    return logger;
}
} 
