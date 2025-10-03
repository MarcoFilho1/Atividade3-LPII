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


add_executable(chat_server
    ${CMAKE_SOURCE_DIR}/src/server/main_server.cpp
)
target_include_directories(chat_server PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(chat_server PRIVATE tslog Threads::Threads)
set_target_properties(chat_server PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_executable(chat_client
    ${CMAKE_SOURCE_DIR}/src/client/main_client.cpp
)
target_include_directories(chat_client PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(chat_client PRIVATE tslog Threads::Threads)
set_target_properties(chat_client PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

