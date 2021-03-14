#include "instructions.h"

#include "crisp8_private.h"
#include "stack.h"

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

// 00E0
// Clears the screen
static void opClearScreen (chip8 emulator)
{
    memset (emulator->display, 0, sizeof (emulator->display) / sizeof (emulator->display [0]));
}

// 1NNN
// Unconditional jump
static void opJump (uint16_t instruction, chip8 emulator)
{
    emulator->PC = INSTRUCTION_GET_NNN (instruction);
}

// 2NNN
// Jump to subroutine
static void opJumpToSubroutine (uint16_t instruction, chip8 emulator)
{
    // I'm currently not doing anything against stack over/underflow even though the stack supports it.
    // I'm considering doing something along the lines of adding internal flags to the chip8 struct that will signal
    // these sorts of things so a frontend can warn about it in a debugger.
    // A stack under/overflow should only occur as the result of a faulty rom so I think it makes sense to leave
    // it to a debugger.
    crisp8StackPush (emulator->stack, emulator->PC);
    emulator->PC = INSTRUCTION_GET_NNN (instruction);
}

// 00EE
// Return from subroutine
static void opReturnFromSubroutine (chip8 emulator)
{
    uint16_t returnAddress;
    crisp8StackPop(emulator->stack, &returnAddress);
    emulator->PC = returnAddress;
}

// 3XNN
static void opSkipIfEqualImmediate (uint16_t instruction, chip8 emulator)
{
    uint8_t registerNum = INSTRUCTION_GET_X (instruction);
    uint8_t compareValue = INSTRUCTION_GET_NN (instruction);
    if (emulator->V [registerNum] == compareValue)
    {
        emulator->PC += 2;
    }
}

// 4XNN
static void opSkipIfNotEqualImmediate (uint16_t instruction, chip8 emulator)
{
    uint8_t registerNum = INSTRUCTION_GET_X (instruction);
    uint8_t compareValue = INSTRUCTION_GET_NN (instruction);
    if (emulator->V [registerNum] != compareValue)
    {
        emulator->PC += 2;
    }
}

// 5XY0
static void opSkipIfEqualRegisters (uint16_t instruction, chip8 emulator)
{
    uint8_t registerX = INSTRUCTION_GET_X (instruction);
    uint8_t registerY = INSTRUCTION_GET_Y (instruction);
    if (emulator->V [registerX] == emulator->V [registerY])
    {
        emulator->PC += 2;
    }
}

// 9XY0
static void opSkipIfNotEqualRegisters (uint16_t instruction, chip8 emulator)
{
    uint8_t registerX = INSTRUCTION_GET_X (instruction);
    uint8_t registerY = INSTRUCTION_GET_Y (instruction);
    if (emulator->V [registerX] != emulator->V [registerY])
    {
        emulator->PC += 2;
    }
}

// 6XNN
// Sets a register to an immediate value
static void opSetVXImmediate (uint16_t instruction, chip8 emulator)
{
    emulator->V [INSTRUCTION_GET_X (instruction)] = INSTRUCTION_GET_NN (instruction);
}

// 7XNN
// Adds an immediate value to a register
static void opAddVXImmediate (uint16_t instruction, chip8 emulator)
{
    emulator->V [INSTRUCTION_GET_X (instruction)] += INSTRUCTION_GET_NN (instruction);
}

// 8XY0
static void opSetVXRegister (uint16_t instruction, chip8 emulator)
{
    emulator->V [INSTRUCTION_GET_X (instruction)] = emulator->V [INSTRUCTION_GET_Y (instruction)];
}

// 8XY1
static void opOr (uint16_t instruction, chip8 emulator)
{
    emulator->V [INSTRUCTION_GET_X (instruction)] |= emulator->V [INSTRUCTION_GET_Y (instruction)];
}

// 8XY2
static void opAnd (uint16_t instruction, chip8 emulator)
{
    emulator->V [INSTRUCTION_GET_X (instruction)] &= emulator->V [INSTRUCTION_GET_Y (instruction)];
}

// 8XY3
static void opXor (uint16_t instruction, chip8 emulator)
{
    emulator->V [INSTRUCTION_GET_X (instruction)] ^= emulator->V [INSTRUCTION_GET_Y (instruction)];
}

// 8XY4
static void opAddVXRegister (uint16_t instruction, chip8 emulator)
{
    uint8_t valueVX = emulator->V [INSTRUCTION_GET_X (instruction)];
    uint8_t valueVY = emulator->V [INSTRUCTION_GET_Y (instruction)];

    // The flag register is set in the case of an integer overflow
    if (valueVY > 255 - valueVX)
    {
        emulator->V [0xF] = 1;
    }
    else
    {
        emulator->V [0xF] = 0;
    }

    emulator->V [INSTRUCTION_GET_X (instruction)] += valueVY;
}

// 8XY5
static void opSubVY (uint16_t instruction, chip8 emulator)
{
    uint8_t minuend = emulator->V [INSTRUCTION_GET_X (instruction)];
    uint8_t subtrehend = emulator->V [INSTRUCTION_GET_Y (instruction)];

    if (minuend > subtrehend)
    {
        emulator->V [0xF] = 1;
    }
    else
    {
        emulator->V [0XF] = 0;
    }

    emulator->V [INSTRUCTION_GET_X (instruction)] = minuend - subtrehend;
}

// 8XY7
static void opSubVX (uint16_t instruction, chip8 emulator)
{
    uint8_t minuend = emulator->V [INSTRUCTION_GET_Y (instruction)];
    uint8_t subtrehend = emulator->V [INSTRUCTION_GET_X (instruction)];

    if (minuend > subtrehend)
    {
        emulator->V [0xF] = 1;
    }
    else
    {
        emulator->V [0XF] = 0;
    }

    emulator->V [INSTRUCTION_GET_X (instruction)] = minuend - subtrehend;
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
            if (NTH_BIT (sprite, 7))
            {
                if (emulator->display [(j * CRISP8_DISPLAY_WIDTH) + i] == 0xFF)
                {
                    emulator->display [(j * CRISP8_DISPLAY_WIDTH) + i] -= 1;
                    emulator->V [0xF] = 1;
                }
                else
                {
                    emulator->display [(j * CRISP8_DISPLAY_WIDTH) + i] = 0xFF;
                }
            }
#else
            if (NTH_BIT (sprite, 0))
            {
                if (emulator->display [(j * CRISP8_DISPLAY_WIDTH) + i])
                {
                    emulator->display [(j * CRISP8_DISPLAY_WIDTH) + i] = 0;
                    emulator->V [0xF] = 1;
                }
                else
                {
                    emulator->display [(j * CRISP8_DISPLAY_WIDTH) + i] = 1;
                }
            }
#endif
            sprite = sprite << 1;
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
            opReturnFromSubroutine (emulator);
            break;
    }
}

static void decodeType8 (uint16_t instruction, chip8 emulator)
{
    // The instructions in this group are differentiated by the last nibble
    switch (INSTRUCTION_GET_NIBBLE (instruction, 3))
    {
        case 0:
            opSetVXRegister (instruction, emulator);
            break;
        case 1:
            opOr (instruction, emulator);
            break;
        case 2:
            opAnd (instruction, emulator);
            break;
        case 3:
            opXor (instruction, emulator);
            break;
        case 4:
            opAddVXRegister (instruction, emulator);
            break;
        case 5:
            opSubVY (instruction, emulator);
            break;
        case 7:
            opSubVX (instruction, emulator);
            break;
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
            opJumpToSubroutine (instruction, emulator);
            break;
        case 3:
            opSkipIfEqualImmediate (instruction, emulator);
            break;
        case 4:
            opSkipIfNotEqualImmediate (instruction, emulator);
            break;
        case 5:
            opSkipIfEqualRegisters (instruction, emulator);
            break;
        case 6:
            opSetVXImmediate (instruction, emulator);
            break;
        case 7:
            opAddVXImmediate (instruction, emulator);
            break;
        case 8:
            decodeType8 (instruction, emulator);
            break;
        case 9:
            opSkipIfNotEqualRegisters (instruction, emulator);
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
