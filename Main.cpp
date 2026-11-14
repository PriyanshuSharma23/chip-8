#include <iostream>
#include <spdlog/spdlog.h>
#include <chrono>

#include "Chip8.h"
#include "Platform.h"

int main(int argc, char **argv)
{

    if (argc != 4)
    {
        spdlog::error("Usage: <SCALE> <DELAY> <ROM path>");
        return 1;
    }

    uint32_t scale = std::stoi(argv[1]);
    uint32_t delay = std::stoi(argv[2]);
    const char *filepath = argv[3];

    Chip8 chip8;
    chip8.LoadROM(filepath);
    int videoPitch = sizeof(chip8.video[0]) * VIDEO_WIDTH;

    Platform platform("Chip8 Emulator", VIDEO_WIDTH * scale, VIDEO_HEIGHT * scale, VIDEO_WIDTH, VIDEO_HEIGHT);

    auto lastCycleTime = std::chrono::high_resolution_clock::now();
    bool quit = false;

    while (!quit)
    {
        quit = platform.ProcessInput(chip8.keypad);

        auto currentTime = std::chrono::high_resolution_clock::now();
        float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();

        if (dt > delay)
        {
            chip8.Cycle();
            platform.Update(chip8.video, videoPitch);
        }
    }

    return 0;
}