#pragma once

#include <cstdint>

const unsigned int VIDEO_WIDTH = 64;
const unsigned int VIDEO_HEIGHT = 32;
const unsigned int STARTING_ADDRESS = 0x200;
const unsigned int FONTSET_STARTING_ADDRESS = 0x050;
const unsigned int FONTSET_SIZE = 16 * 5; // 16 chars, 5 bytes each for sprite

class Chip8
{
public:
    uint8_t registers[16]{};
    uint8_t memory[4096]{};
    uint16_t index{};
    uint16_t pc{};
    uint16_t stack[16]{};
    uint8_t sp{};
    uint8_t delayTimer{};
    uint8_t sountTimer{};
    uint8_t keypad[16]{};
    uint32_t video[VIDEO_HEIGHT * VIDEO_WIDTH]{};
    uint16_t opcode;

    Chip8();
    void LoadROM(const char *filepath);

private:
};
