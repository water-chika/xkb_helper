cmake_minimum_required(VERSION 3.20)

project(xkb_helper)

add_library(xkb_helper
    xkb_helper.hpp
    xkb_helper.cpp
)

target_link_libraries(xkb_helper PUBLIC
    cpp_helper
    xkbcommon
)
target_include_directories(xkb_helper PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

