cmake_minimum_required(VERSION 3.20)
project(chat_tcp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Dependência: libtslog (header-only) com CMake próprio
add_subdirectory(libs/libtslog)

# Binário de teste de estresse
add_executable(log_stress
    ${CMAKE_SOURCE_DIR}/tools/log_stress.cpp
)

target_include_directories(log_stress PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

find_package(Threads REQUIRED)
target_link_libraries(log_stress PRIVATE tslog Threads::Threads)

# Pasta de saída dos binários (opcional)
set_target_properties(log_stress PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)
