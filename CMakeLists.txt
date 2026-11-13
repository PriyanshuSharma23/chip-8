cmake_minimum_required(VERSION 3.16)
project(chip8)

add_subdirectory(vendored/spdlog EXCLUDE_FROM_ALL)

add_executable(chip8 
    Main.cpp
    Chip8.cpp)

target_link_libraries(chip8 PRIVATE spdlog::spdlog)