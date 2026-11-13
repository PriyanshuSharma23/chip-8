# Chip 8 Emulator
* Goal is to make an emulator for the 8 bit CHIP8
* Hexadecimals begin with $* or 0x*, ex: $22 or 0x22

## Description

### Components

1. 16 8-bit registers
    * Registers used by CPU to perform operations, long term storage done in RAM.

2. 4K bytes of memory
    * Address space is between 0x000 to 0xFFF
    * Address spaces and their significance
        * 0x000 - 0x1FF: Space to keep the chip 8 interpertter, we will not put anything here except for...
        * 0x050 - 0x0A0: Store the 16 characters from 0 to F, required by ROMs
        * 0x200 - 0xFFF: Instructions from ROM will start at 0x200, anything left free after ROM's space is free to use

3. 16 Bit Index register
    * Used to store memory addresses for operations
    * 16 Bit because 0xFFF is too large to fit in 8 bit register

4. 16 Bit Program counter
    * Holds the address of next instruction that needs to be executed
    * An instruction is of 2 bytes but the PC points to 1 byte, so when we read an instruction we read 
      PC and PC + 1, increment the PC by 2 (PC + 2)

5. 16 level stack 
It is used to store the PC (call stack), modified with instructions like `CALL` (pushhing SP + 1), and `RET` (popping SP-1). 

6. 8 bit stack pointer
Stores the index at which the next PC value should be stored in the stack. 

Push operation -> PC = PC + 2 -> push $PC in stack @ SP -> SP = SP + 1
Pop operation -> SP = SP - 1 -> goto $PC instruction 

7. 8 bit delay Timer
A value X will be decremented by 1 @60Hz (Or @cycle clock Hz)

8. 8 bit sound Timer
Similar to 8 bit delay timer, but it emitted a sound whenever it decremented the value

9. 16 input keys
Here is how to map the 16 input keys (0-F)
```
Keypad       Keyboard
+-+-+-+-+    +-+-+-+-+
|1|2|3|C|    |1|2|3|4|
+-+-+-+-+    +-+-+-+-+
|4|5|6|D|    |Q|W|E|R|
+-+-+-+-+ => +-+-+-+-+
|7|8|9|E|    |A|S|D|F|
+-+-+-+-+    +-+-+-+-+
|A|0|B|F|    |Z|X|C|V|
+-+-+-+-+    +-+-+-+-+
```

The 4 key is left rotate, 5 - left move, 6 - right move, 1
- drop, ENTER - restart, DROP - end.  After every 5 lines, the speed
increases slightly and peaks at 45 lines.

10. 64x32 Monochrome Display Memory

*  A sprite XORs the current display



