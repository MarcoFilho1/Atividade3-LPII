cmake_minimum_required(VERSION 3.20)
project(chat_tcp LANGUAGES CXX)

# C++20 (std::jthread, std::counting_semaphore)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(tslog STATIC
    src/tslog/logger.cpp
    src/tslog/sinks.cpp
)
target_include_directories(tslog PUBLIC include)
if (MSVC)
  target_compile_options(tslog PRIVATE /W4 /permissive- /EHsc)
else()
  target_compile_options(tslog PRIVATE -Wall -Wextra -Wpedantic -Wconversion)
endif()

add_executable(log_stress examples/log_stress.cpp)
target_link_libraries(log_stress PRIVATE tslog)

include(GNUInstallDirs)
install(TARGETS tslog ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
