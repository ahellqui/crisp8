#include "instructions.h"

#include "crisp8_private.h"

#include <string.h>

// Macros to extract values out of instructions

// Chip-8 is big endian so the nibbles are counted from the left.
// Also, this counts from 0
#define INSTRUCTION_GET_NIBBLE(instruction, nibble) (((instruction) & (0xF000 >> ((nibble) * 4))) \
                                                                        >> (12 - ((nibble) * 4)))

#define INSTRUCTION_GET_X(instruction) (INSTRUCTION_GET_NIBBLE ((instruction), 1))
#define INSTRUCTION_GET_Y(instruction) (INSTRUCTION_GET_NIBBLE ((instruction), 2))

#define INSTRUCTION_GET_N(instruction)   ((instruction) & 0x000F)
#define INSTRUCTION_GET_NN(instruction)  ((instruction) & 0x00FF)
#define INSTRUCTION_GET_NNN(instruction) ((instruction) & 0x0FFF)

// General purpose macros

// This is for the computer representation of bytes, meaning that bits are now counted from the right.
// This also counts from 0
#define NTH_BIT(num, n) (((num) & (1 << (n))) >> (n))

uint16_t fetchInstruction (chip8 emulator)
{
    uint16_t instruction = 0;
    // I'm not quite sure how endiannes work with bitwise operators, but hopefully this still works on non little endian
    // machines.
    instruction = (uint16_t)emulator->memory [emulator->PC] << 8;
    instruction |= (uint16_t)emulator->memory [emulator->PC + 1];

    // Increment PC for the next instruction
    emulator->PC += 2;

    return instruction;
}

// Instructions (starts with op for opcode) ----------------------------
// There are brief comments of what they do, but honestly just look up the opcodes on wikipedia or something if you want
// details

// Clears the screen
static void opClearScreen (chip8 emulator)
{
    memset (emulator->display, 0, sizeof (emulator->display) / sizeof (emulator->display [0]));
}

// Unconditional jump
static void opJump (uint16_t instruction, chip8 emulator)
{
    emulator->PC = INSTRUCTION_GET_NNN (instruction);
}

// Sets a register to an immediate value
static void opSetVX (uint16_t instruction, chip8 emulator)
{
    emulator->V [INSTRUCTION_GET_X (instruction)] = INSTRUCTION_GET_NN (instruction);
}

// Adds an immediate value to a register
static void opAddImmediate (uint16_t instruction, chip8 emulator)
{
    emulator->V [INSTRUCTION_GET_X (instruction)] += INSTRUCTION_GET_NN (instruction);
}

// Sets the index register to an immediate value
static void opSetIndex (uint16_t instruction, chip8 emulator)
{
    emulator->I = INSTRUCTION_GET_NNN (instruction);
}

// Draws to the screen
static void opDraw (uint16_t instruction, chip8 emulator)
{
    emulator->V [0xF] = 0;

    uint8_t xCoord = emulator->V [INSTRUCTION_GET_X (instruction)] % CRISP8_DISPLAY_WIDTH;
    uint8_t yCoord = emulator->V [INSTRUCTION_GET_Y (instruction)] % CRISP8_DISPLAY_HEIGHT;
    uint8_t height = INSTRUCTION_GET_N (instruction);

    uint16_t spriteAddress = emulator->I;

    for (int j = yCoord; j < yCoord + height && j < CRISP8_DISPLAY_HEIGHT; j++)
    {
        uint8_t sprite = emulator->memory [spriteAddress];

        // xCoord + 8 since a sprite is represented by an 8 bit byte
        for (int i = xCoord; i < xCoord + 8 && i < CRISP8_DISPLAY_WIDTH; i++)
        {
#ifdef CRISP8_DISPLAY_USE_ALPHA
            if (NTH_BIT (sprite, 0))
            {
                if (emulator->display [(j * CRISP8_DISPLAY_HEIGHT) + i] == 0xFF)
                {
                    emulator->display [(j * CRISP8_DISPLAY_HEIGHT) + i] -= 1;
                    emulator->V [0xF] = 1;
                }
                else
                {
                    emulator->display [(j * CRISP8_DISPLAY_HEIGHT) + i] = 0xFF;
                }
            }
#else
            if (NTH_BIT (sprite, 0))
            {
                if (emulator->display [(j * CRISP8_DISPLAY_HEIGHT) + i])
                {
                    emulator->display [(j * CRISP8_DISPLAY_HEIGHT) + i] = 0;
                    emulator->V [0xF] = 1;
                }
                else
                {
                    emulator->display [(j * CRISP8_DISPLAY_HEIGHT) + i] = 1;
                }
            }
#endif
            sprite = sprite >> 1;
        }

        spriteAddress += 1;
    }
}

// Instruction decoding ------------------------------------------------

static void decodeType0 (uint16_t instruction, chip8 emulator)
{
    // I'm not including the 0NNN instruction, so no instructions have operands
    switch (instruction)
    {
        case 0x00E0:
            opClearScreen (emulator);
            break;
        case 0x00EE:
            // Return from subroutine
            break;
    }
}

void dispatchInstruction (uint16_t instruction, chip8 emulator)
{
    // Instructions on the chip8 are divided into types by the first nibble. If an instruction is alone in its type, it
    // just gets executed, otherwise it is sent to a decoding function for further decoding.
    switch (INSTRUCTION_GET_NIBBLE (instruction, 0))
    {
        case 0:
            decodeType0 (instruction, emulator);
            break;
        case 1:
            opJump (instruction, emulator);
            break;
        case 2:
            break;
        case 3:
            break;
        case 6:
            opSetVX (instruction, emulator);
            break;
        case 7:
            opAddImmediate (instruction, emulator);
            break;
        case 8:
            break;
        case 0xA:
            opSetIndex (instruction, emulator);
            break;
        case 0xB:
            break;
        case 0xC:
            break;
        case 0xD:
            opDraw (instruction, emulator);
            break;
        case 0xE:
            break;
        case 0xF:
            break;
    }
}
