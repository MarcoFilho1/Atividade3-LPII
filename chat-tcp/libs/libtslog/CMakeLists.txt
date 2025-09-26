# Biblioteca header-only chamada "tslog"
add_library(tslog INTERFACE)
target_include_directories(tslog INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
