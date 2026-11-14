cmake_minimum_required(VERSION 3.16)
project(chip8)

add_subdirectory(vendored/spdlog EXCLUDE_FROM_ALL)
add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)

add_executable(chip8 
    Main.cpp
    Chip8.cpp
    Chip8.h
    Platform.cpp
    Platform.h)

target_link_libraries(chip8 PRIVATE spdlog::spdlog)
target_link_libraries(chip8 PRIVATE SDL3::SDL3)