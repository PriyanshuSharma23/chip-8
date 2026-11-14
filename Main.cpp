#include <iostream>
#include <spdlog/spdlog.h>
#include <chrono>

#include "Chip8.h"
#include "Platform.h"

int main(int argc, char **argv)
{
    spdlog::info("Starting CHIP-8 emulator");

    if (argc != 4)
    {
        spdlog::error("Invalid argument count: expected 3 arguments, received {}", argc - 1);
        spdlog::error("Usage: {} <SCALE> <DELAY> <ROM path>", argv[0]);
        return 1;
    }

    uint32_t scale = std::stoi(argv[1]);
    uint32_t delay = std::stoi(argv[2]);
    const char *filepath = argv[3];

    spdlog::info("Configuration: scale={}, cycle delay={} ms, ROM='{}'", scale, delay, filepath);

    Chip8 chip8;
    chip8.LoadROM(filepath);
    int videoPitch = sizeof(chip8.video[0]) * VIDEO_WIDTH;

    spdlog::info(
        "Video buffer configured: {}x{}, {} bytes per pixel, {} bytes per row",
        VIDEO_WIDTH,
        VIDEO_HEIGHT,
        sizeof(chip8.video[0]),
        videoPitch);

    Platform platform("Chip8 Emulator", VIDEO_WIDTH * scale, VIDEO_HEIGHT * scale, VIDEO_WIDTH, VIDEO_HEIGHT);

    spdlog::info("Entering emulation loop");

    auto lastCycleTime = std::chrono::high_resolution_clock::now();
    bool quit = false;
    uint64_t cycleCount = 0;

    while (!quit)
    {
        quit = platform.ProcessInput(chip8.keypad);

        auto currentTime = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();

        if (dt > delay)
        {
            chip8.Cycle();
            platform.Update(chip8.video, videoPitch);
            ++cycleCount;
        }
    }

    spdlog::info("Stopping emulator after {} cycles", cycleCount);

    return 0;
}
