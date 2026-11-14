#include "Chip8.h"

#include <fstream>
#include <spdlog/spdlog.h>
#include <cerrno>
#include <cstring>

uint8_t fontset[FONTSET_SIZE] =
    {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

Chip8::Chip8()
{
    pc = STARTING_ADDRESS;
    std::memcpy(memory + FONTSET_STARTING_ADDRESS, fontset, FONTSET_SIZE);

    // initialize tables
    table[0x0] = &Table0;
    table[0x1] = &OP_1nnn;
    table[0x2] = &OP_2nnn;
    table[0x3] = &OP_3xkk;
    table[0x4] = &OP_4xkk;
    table[0x5] = &OP_5xy0;
    table[0x6] = &OP_6xkk;
    table[0x7] = &OP_7xkk;
    table[0x8] = &Table8;
    table[0x9] = &OP_9xy0;
    table[0xA] = &OP_Annn;
    table[0xB] = &OP_Bnnn;
    table[0xC] = &OP_Cxkk;
    table[0xD] = &OP_Dxyn;
    table[0xE] = &TableE;
    table[0xF] = &TableF;
    

    // fill table 0
    for (unsigned int i = 0; i < 0xE + 1; i++)
    {
        table0[i] = &OP_NULL;
    }
    table0[0x0] = &OP_00E0;
    table0[0xE] = &OP_00EE;
    

    // fill table 8
    for (unsigned int i = 0; i < 0xE + 1; i++)
    {
        table8[i] = &OP_NULL;
    }
    table8[0x0] = &OP_8xy0;
    table8[0x1] = &OP_8xy1;
    table8[0x2] = &OP_8xy2;
    table8[0x3] = &OP_8xy3;
    table8[0x4] = &OP_8xy4;
    table8[0x5] = &OP_8xy5;
    table8[0x6] = &OP_8xy6;
    table8[0x7] = &OP_8xy7;
    table8[0xE] = &OP_8xyE;
    
    // fill table E
    for (unsigned int i = 0; i < 0xE + 1; i++)
    {
        tableE[i] = &OP_NULL;
    }
    tableE[0x1] = &OP_ExA1;
    tableE[0xE] = &OP_Ex9E;
    
    // fill table F
    for (unsigned int i = 0; i < 0x65 + 1; i++)
    {
        tableF[i] = &OP_NULL;
    }
    tableF[0x07] = &OP_Fx07;
    tableF[0x0A] = &OP_Fx0A;
    tableF[0x15] = &OP_Fx15;
    tableF[0x18] = &OP_Fx18;
    tableF[0x1E] = &OP_Fx1E;
    tableF[0x29] = &OP_Fx29;
    tableF[0x33] = &OP_Fx33;
    tableF[0x55] = &OP_Fx55;
    tableF[0x65] = &OP_Fx65;
}

void Chip8::LoadROM(const char *filepath)
{
    spdlog::info("Loading ROM from {}", filepath);
    std::ifstream file(filepath, std::ios::binary | std::ios::ate);

    if (!file.is_open())
    {
        spdlog::error("failed to open ROM file at '{}': {}", filepath, std::strerror(errno));
        return;
    }

    std::streampos size = file.tellg();
    char *buffer = new char[size];

    file.seekg(std::ios::beg);
    file.read(buffer, size);

    spdlog::info("copying over {} bytes to memory", (int)size);
    std::memcpy(memory + STARTING_ADDRESS, buffer, size);

    delete[] buffer;
}

// CLS
// Clear the display.
void Chip8::OP_00E0()
{
    std::memset(video, 0, sizeof(video));
}

// RET
// Return from a subroutine.The interpreter sets the program counter to the address at the top of the stack,
// then subtracts 1 from the stack pointer.
void Chip8::OP_00EE()
{
    if (sp == 0)
    {
        spdlog::error("stack underflow");
        return;
    }
    sp--;
    pc = stack[sp];
}

// JP addr
// Jump to location nnn. The interpreter sets the program counter to nnn.
void Chip8::OP_1nnn()
{
    uint16_t addr = opcode & 0x0FFFu;
    pc = addr;
}

// CALL addr
// Call subroutine at nnn. The interpreter increments the stack pointer, then puts the current PC on the top
// of the stack. The PC is then set to nnn.
void Chip8::OP_2nnn()
{
    if (sp >= 16)
    {
        spdlog::error("stack overflow");
        return;
    }
    uint16_t addr = opcode & 0x0FFFu;
    stack[sp] = pc;
    sp += 1;
    pc = addr;
}

// 3xkk - SE Vx, byte
// Skip next instruction if Vx = kk. The interpreter compares register Vx to kk, and if they are equal,
// increments the program counter by 2.
void Chip8::OP_3xkk()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    if (registers[Vx] == byte)
    {
        pc += 2;
    }
}

// 4xkk - SNE Vx, byte
// Skip next instruction if Vx != kk. The interpreter compares register Vx to kk, and if they are not equal,
// increments the program counter by 2.
void Chip8::OP_4xkk()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    if (registers[Vx] != byte)
    {
        pc += 2;
    }
}

// 5xy0 - SE Vx, Vy
// Skip next instruction if Vx = Vy. The interpreter compares register Vx to register Vy, and if they are equal,
// increments the program counter by 2.
void Chip8::OP_5xy0()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if (registers[Vx] == registers[Vy])
    {
        pc += 2;
    }
}

// 6xkk - LD Vx, byte
// Set Vx = kk. The interpreter puts the value kk into register Vx.
void Chip8::OP_6xkk()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;
    registers[Vx] = byte;
}

// 7xkk - ADD Vx, byte
// Set Vx = Vx + kk. Adds the value kk to the value of register Vx, then stores the result in Vx.
void Chip8::OP_7xkk()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;
    registers[Vx] += byte;
}

// 8xy0 - LD Vx, Vy
// Set Vx = Vy. Stores the value of register Vy in register Vx.
void Chip8::OP_8xy0()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    registers[Vx] = registers[Vy];
}

// 8xy1 - OR Vx, Vy
// Set Vx = Vx OR Vy. Performs a bitwise OR on the values of Vx and Vy, then stores the result in Vx. A
// bitwise OR compares the corresponding bits from two values, and if either bit is 1, then the same bit in the
// result is also 1. Otherwise, it is 0.
void Chip8::OP_8xy1()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    registers[Vx] |= registers[Vy];
}

// 8xy2 - AND Vx, Vy
// Set Vx = Vx AND Vy. Performs a bitwise AND on the values of Vx and Vy, then stores the result in Vx.
// A bitwise AND compares the corresponding bits from two values, and if both bits are 1, then the same bit
// in the result is also 1. Otherwise, it is 0.
void Chip8::OP_8xy2()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    registers[Vx] &= registers[Vy];
}

// 8xy3 - XOR Vx, Vy
// Set Vx = Vx XOR Vy. Performs a bitwise exclusive OR on the values of Vx and Vy, then stores the result
// in Vx. An exclusive OR compares the corresponding bits from two values, and if the bits are not both the
// same, then the corresponding bit in the result is set to 1. Otherwise, it is 0.
void Chip8::OP_8xy3()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    registers[Vx] ^= registers[Vy];
}

// 8xy4 - ADD Vx, Vy
// Set Vx = Vx + Vy, set VF = carry. The values of Vx and Vy are added together. If the result is greater
// than 8 bits (i.e., ¿ 255,) VF is set to 1, otherwise 0. Only the lowest 8 bits of the result are kept, and stored
// in Vx.
void Chip8::OP_8xy4()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    uint16_t sum = registers[Vx] + registers[Vy];

    registers[Vx] = sum & 0xFF;

    if (sum > 255U)
    {
        registers[0xF] = 1;
    }
    else
    {
        registers[0xF] = 0;
    }
}

// 8xy5 - SUB Vx, Vy
// Set Vx = Vx - Vy, set VF = NOT borrow. If Vx ¿ Vy, then VF is set to 1, otherwise 0. Then Vy is
// subtracted from Vx, and the results stored in Vx.
// Flag(VF)
// If Vx >= Vy (no borrow / no underflow), VF is set to 1.
// If Vx < Vy (borrow / underflow occurs), VF is set to 0.
void Chip8::OP_8xy5()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    bool notBorrow = registers[Vx] >= registers[Vy];

    registers[Vx] -= registers[Vy];

    if (notBorrow)
    {
        registers[0xF] = 1;
    }
    else
    {
        registers[0xF] = 0;
    }
}

// 8xy6 - SHR Vx {, Vy}
// Set Vx = Vx SHR 1. If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0. Then Vx is
// divided by 2.
void Chip8::OP_8xy6()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t shiftedValue = registers[Vx] & 0x1u;
    registers[Vx] >>= 1;
    registers[0xF] = shiftedValue;
}

// 8xy7 - SUBN Vx, Vy
// Set Vx = Vy - Vx, set VF = NOT borrow. If Vy ¿ Vx, then VF is set to 1, otherwise 0. Then Vx is
// subtracted from Vy, and the results stored in Vx.
void Chip8::OP_8xy7()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    bool notBorrow = registers[Vy] >= registers[Vx];

    registers[Vx] = registers[Vy] - registers[Vx];

    if (notBorrow)
    {
        registers[0xF] = 1;
    }
    else
    {
        registers[0xF] = 0;
    }
}

// 8xyE - SHL Vx {, Vy}
// Set Vx = Vx SHL 1. If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0. Then Vx is
// multiplied by 2.
void Chip8::OP_8xyE()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t shiftedValue = (registers[Vx] & 0x80) >> 7u;
    registers[Vx] <<= 1;
    registers[0xF] = shiftedValue;
}

// 9xy0 - SNE Vx, Vy
// Skip next instruction if Vx != Vy. The values of Vx and Vy are compared, and if they are not equal, the
// program counter is increased by 2.
void Chip8::OP_9xy0()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if (registers[Vx] != registers[Vy])
    {
        pc += 2;
    }
}

// Annn - LD I, addr
// Set I = nnn. The value of register I is set to nnn
void Chip8::OP_Annn()
{
    uint16_t addr = opcode & 0x0FFFu;
    index = addr;
}

// Bnnn - JP V0, addr
// Jump to location nnn + V0. The program counter is set to nnn plus the value of V0.
void Chip8::OP_Bnnn()
{
    uint16_t addr = opcode & 0x0FFFu;
    pc = registers[0] + addr;
}

// Cxkk - RND Vx, byte
// Set Vx = random byte AND kk. The interpreter generates a random number from 0 to 255, which is then
// ANDed with the value kk. The results are stored in Vx. See instruction 8xy2 for more information on AND.
void Chip8::OP_Cxkk()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;
    registers[Vx] = (uint8_t)distribution(generator) & byte;
}

// Dxyn - DRW Vx, Vy, nibble
// Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision. The interpreter reads n
// bytes from memory, starting at the address stored in I. These bytes are then displayed as sprites on screen
// at coordinates (Vx, Vy). Sprites are XOR’d onto the existing screen. If this causes any pixels to be erased,
// VF is set to 1, otherwise it is set to 0. If the sprite is positioned so part of it is outside the coordinates of
// the display, it wraps around to the opposite side of the screen.
void Chip8::OP_Dxyn()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    uint8_t height = opcode & 0x000Fu;

    uint8_t xPos = registers[Vx] % VIDEO_WIDTH;
    uint8_t yPos = registers[Vy] % VIDEO_HEIGHT;

    registers[0xF] = 0;
    for (unsigned int row = 0; row < height; ++row)
    {
        uint8_t spriteByte = memory[index + row];

        for (unsigned int col = 0; col < 8; ++col)
        {
            bool pixel = spriteByte & (0x80u >> col);
            uint32_t *displayPixel =
                &video[((yPos + row) % VIDEO_HEIGHT) * VIDEO_WIDTH + ((xPos + col) % VIDEO_WIDTH)];

            if (pixel)
            {
                if (*displayPixel == 0xFFFFFFFF)
                {
                    // collision
                    registers[0xF] = 1;
                }

                *displayPixel ^= 0xFFFFFFFF;
            }
        }
    }
}

// Ex9E - SKP Vx
// Skip next instruction if key with the value of Vx is pressed. Checks the keyboard, and if the key corresponding
// to the value of Vx is currently in the down position, PC is increased by 2.
void Chip8::OP_Ex9E()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t key = registers[Vx];

    if (key < 16 && keypad[key])
    {
        pc += 2;
    }
}

// ExA1 - SKNP Vx
// Skip next instruction if key with the value of Vx is not pressed. Checks the keyboard, and if the key
// corresponding to the value of Vx is currently in the up position, PC is increased by 2.
void Chip8::OP_ExA1()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t key = registers[Vx];

    if (key < 16 && !keypad[key])
    {
        pc += 2;
    }
}

// Fx07 - LD Vx, DT
// Set Vx = delay timer value. The value of DT is placed into Vx.
void Chip8::OP_Fx07()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    registers[Vx] = delayTimer;
}

// Fx0A - LD Vx, K
// Wait for a key press, store the value of the key in Vx. All execution stops until a key is pressed, then the
// value of that key is stored in Vx.
void Chip8::OP_Fx0A()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    // check for any key pressed
    for (unsigned int key = 0; key < 16; ++key)
    {
        if (keypad[key])
        {
            registers[Vx] = key;
            return;
        }
    }

    pc -= 2;
}

// Fx15 - LD DT, Vx
// Set delay timer = Vx. Delay Timer is set equal to the value of Vx
void Chip8::OP_Fx15()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    delayTimer = registers[Vx];
}

// Fx18 - LD ST, Vx
// Set sound timer = Vx. Sound Timer is set equal to the value of Vx.
void Chip8::OP_Fx18()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    soundTimer = registers[Vx];
}

// Fx1E - ADD I, Vx
// Set I = I + Vx. The values of I and Vx are added, and the results are stored in I.
void Chip8::OP_Fx1E()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    index += registers[Vx];
}

// Fx29 - LD F, Vx
// Set I = location of sprite for digit Vx. The value of I is set to the location for the hexadecimal sprite
// corresponding to the value of Vx. See section 2.4, Display, for more information on the Chip-8 hexadecimal
// font. To obtain this value, multiply VX by 5 (all font data stored in first 80 bytes of memory).
void Chip8::OP_Fx29()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    index = FONTSET_STARTING_ADDRESS + (registers[Vx] * 5);
}

// Fx33 - LD B, Vx
// Store BCD representation of Vx in memory locations I, I+1, and I+2. The interpreter takes the decimal
// value of Vx, and places the hundreds digit in memory at location in I, the tens digit at location I+1, and
// the ones digit at location I+2.
void Chip8::OP_Fx33()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t value = registers[Vx];

    memory[index + 2] = value % 10;
    value /= 10;

    memory[index + 1] = value % 10;
    value /= 10;

    memory[index] = value % 10;
}

// Fx55 - LD [I], Vx
// Stores V0 to VX in memory starting at address I.
void Chip8::OP_Fx55()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    std::memcpy(memory + index, registers, Vx + 1);
}

// Fx65 - LD Vx, [I]
// Fills V0 to VX with values from memory starting at address I.
void Chip8::OP_Fx65()
{
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    std::memcpy(registers, memory + index, Vx + 1);
}

void Chip8::Cycle()
{
    opcode = (memory[pc] << 8u) | memory[pc + 1];
    pc += 2;

    ((*this).*(table[(opcode & 0xF000) >> 12u]))();

    if (delayTimer > 0)
    {
        delayTimer--;
    }

    if (soundTimer > 0)
    {
        soundTimer--;
    }
}

void Chip8::OP_NULL()
{
}