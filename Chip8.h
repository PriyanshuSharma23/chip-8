#pragma once

#include <cstdint>
#include <random>

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
    uint8_t soundTimer{};
    uint8_t keypad[16]{};
    uint32_t video[VIDEO_HEIGHT * VIDEO_WIDTH]{};
    uint16_t opcode;

    Chip8();
    void LoadROM(const char *filepath);
    
    void OP_00E0();
    void OP_00EE();
    void OP_1nnn();
    void OP_2nnn();
    void OP_3xkk();
    void OP_4xkk();
    void OP_5xy0();
    void OP_6xkk();
    void OP_7xkk();
    void OP_8xy0();
    void OP_8xy1();
    void OP_8xy2();
    void OP_8xy3();
    void OP_8xy4();
    void OP_8xy5();
    void OP_8xy6();
    void OP_8xy7();
    void OP_8xyE();
    void OP_9xy0();
    void OP_Annn();
    void OP_Bnnn();
    void OP_Cxkk();
    void OP_Dxyn();
    void OP_Ex9E();
    void OP_ExA1();
    void OP_Fx07();
    void OP_Fx0A();
    void OP_Fx15();
    void OP_Fx18();
    void OP_Fx1E();
    void OP_Fx29();
    void OP_Fx33();
    void OP_Fx55();
    void OP_Fx65();
    void OP_NULL();
    void Cycle();

private:
    std::mt19937 generator{std::random_device{}()};
    std::uniform_int_distribution<int> distribution{0, 255};

    typedef void (Chip8::*Chip8Func)();
    Chip8Func table[16];
    Chip8Func table0[0xE + 1];
    Chip8Func table8[0xE + 1];
    Chip8Func tableE[0xE + 1];
    Chip8Func tableF[0x65 + 1];
    
    void Table0() 
    {
        ((*this).*(table0[opcode & 0xF]))();
    }

    void Table8() 
    {
        ((*this).*(table8[opcode & 0xF]))();
    }

    void TableE() 
    {
        ((*this).*(tableE[opcode & 0xF]))();
    }

    void TableF() 
    {
        ((*this).*(tableF[opcode & 0xFF]))();
    }
};
