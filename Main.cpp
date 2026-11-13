#include <iostream>
#include <spdlog/spdlog.h>

#include "Chip8.h"

int main(int argc, char **argv) 
{
    
    if (argc != 2)
    {
        spdlog::error("Usage: <ROM path>");
        return 1;
    }
    
    const char* filepath = argv[1];
    
    Chip8 chip8;
    chip8.LoadROM(filepath);

    return 0;
}