cmake_minimum_required(VERSION 3.20)
project(chat_tcp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(libs/libtslog)

add_executable(log_stress tools/log_stress.cpp)
target_include_directories(log_stress PRIVATE src)
target_link_libraries(log_stress PRIVATE tslog pthread)
