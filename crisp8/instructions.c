#include "instructions.h"

#include "crisp8_private.h"
#include "stack.h"

#include <string.h>
#include <stdlib.h>

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
// There are brief comments of what some of them, but honestly just look up the opcodes on wikipedia or something if
// you want details. There are way better references than anything I should write here
//
// All instructoin functions take one or two parameters. All of them take in "emulator" which is the emulator on which
// to operate. Some of them take in "instruction" which is the current fetched instruction for the cases where it
// contains additional information needed by the function.

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

// 8XY6
static void opShiftRight (uint16_t instruction, chip8 emulator)
{
    // The original chip-8 put the value in VY in VX before shifting, but this was later removed.
    // You can choose which behaviour to compile in
    if (crisp8ConfigGetShift (emulator) == OLD)
    {
        emulator->V [INSTRUCTION_GET_X (instruction)] = emulator->V [INSTRUCTION_GET_Y (instruction)];
    }

    uint8_t valueVX = emulator->V [INSTRUCTION_GET_X (instruction)];

    emulator->V [0xF] = NTH_BIT (valueVX, 0);
    emulator->V [INSTRUCTION_GET_X (instruction)] = valueVX >> 1;
}

// 8XYE
static void opShiftLeft (uint16_t instruction, chip8 emulator)
{
    // The original chip-8 put the value in VY in VX before shifting, but this was later removed.
    // You can choose which behaviour to compile in
    if (crisp8ConfigGetShift (emulator) == OLD)
    {
        emulator->V [INSTRUCTION_GET_X (instruction)] = emulator->V [INSTRUCTION_GET_Y (instruction)];
    }

    uint8_t valueVX = emulator->V [INSTRUCTION_GET_X (instruction)];

    emulator->V [0xF] = NTH_BIT (valueVX, 7);
    emulator->V [INSTRUCTION_GET_X (instruction)] = valueVX << 1;
}

// ANNN
// Sets the index register to an immediate value
static void opSetIndex (uint16_t instruction, chip8 emulator)
{
    emulator->I = INSTRUCTION_GET_NNN (instruction);
}

// BNNN/BXNN (depending on config)
// Jump with offset
static void opJumpWithOffset (uint16_t instruction, chip8 emulator)
{
    uint16_t baseAddress = INSTRUCTION_GET_NNN (instruction);
    uint8_t offset;

    if (crisp8ConfigGetJumpOffset (emulator) == OLD)
    {
        offset = emulator->V [0];
    }
    else
    {
        offset = emulator->V [INSTRUCTION_GET_X (instruction)];
    }

    emulator->PC = baseAddress + offset;
}

// CXNN
// Random
static void opRandom (uint16_t instruction, chip8 emulator)
{
    // I don't think a library should call srand, so it has to be clear to the user that it is their responsibility
    uint8_t randomNum = rand () & INSTRUCTION_GET_NN (instruction);
    emulator->V [INSTRUCTION_GET_X (instruction)] = randomNum;
}

// EX9E
// Skip if key is pressed
static void opSkipIfKey (uint16_t instruction, chip8 emulator)
{
    uint32_t keyMap = emulator->inputCb ();
    uint8_t key = emulator->V [INSTRUCTION_GET_X (instruction)];

    // The keymap in defs.h is set up such that the value in VX will be the bit corresponding to its key.
    // Key values from 0x0 to 0xF are allowed; A value outside of this is counted as not pressed
    if (keyMap & (1 << key) && key <= 0xF)
    {
        emulator->PC += 2;
    }
}

// EXA1
// Skip if key is not pressed
static void opSkipIfNotKey (uint16_t instruction, chip8 emulator)
{
    uint32_t keyMap = emulator->inputCb ();
    uint8_t key = emulator->V [INSTRUCTION_GET_X (instruction)];

    // The keymap in defs.h is set up such that the value in VX will be the bit corresponding to its key
    // Key values from 0x0 to 0xF are allowed; A value outside of this is counted as not pressed
    if ((!(keyMap & (1 << key))) || key > 0xF)
    {
        emulator->PC += 2;
    }
}

// DXYN
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
            if (NTH_BIT (sprite, 7))
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

// FX07
// Sets VX to the delay timer
static void opSetVXDelay (uint16_t instruction, chip8 emulator)
{
    emulator->V [INSTRUCTION_GET_X (instruction)] = emulator->delayTimer;
}

// FX15
// Sets the delay timer to VX
static void opSetDelayTimer (uint16_t instruction, chip8 emulator)
{
    emulator->delayTimer = emulator->V [INSTRUCTION_GET_X (instruction)];
}

// FX18
// Sets the delay timer to VX
static void opSetSoundTimer (uint16_t instruction, chip8 emulator)
{
    emulator->soundTimer = emulator->V [INSTRUCTION_GET_X (instruction)];
}

// FX1E
// Add to index
static void opAddToIndex (uint16_t instruction, chip8 emulator)
{
    emulator->I += emulator->V [INSTRUCTION_GET_X (instruction)];

    if (emulator->I > 0x1000)
    {
        emulator->V [0xF] = 1;
    }
}

// FX0A
// Get key (blocking)
static void opGetKey (uint16_t instruction, chip8 emulator)
{
    // A key is registered on release, so we have to do some funky stuff
    uint32_t keyMap = emulator->inputCb ();

    if (!emulator->lastKeyState || keyMap == emulator->lastKeyState)
    {
        emulator->PC -= 2;
    }
    else
    {
        for (int i = 0; i <= 0xF; i++)
        {
            if (NTH_BIT (emulator->lastKeyState, i) && !NTH_BIT (keyMap, i))
            {
                emulator->V [INSTRUCTION_GET_X (instruction)] = i;
                break;
            }
        }
    }
}

// FX29
// Font character
static void opFontCharacter (uint16_t instruction, chip8 emulator)
{
    // The character to use is in the last nibble of VX.
    // The instruction macros won't work here since they are for 32 bit integers
    uint8_t character = emulator->V [INSTRUCTION_GET_X (instruction)] & 0x0F;

    // The font is at the font base address + (character * 5) since every font sprite is 5 pixels tall and therefore
    // occupy 5 bytes
    uint16_t fontAddress = CRISP8_FONT_START_ADDRESS + (character * 5);

    emulator->I = fontAddress;
}

// FX33
// Decimal conversion
static void opDecimalConvert (uint16_t instruction, chip8 emulator)
{
    uint8_t number = emulator->V [INSTRUCTION_GET_X (instruction)];
    for (int i = 2; i >= 0; i--)
    {
        emulator->memory [emulator->I + i] = number % 10;
        number /= 10;
    }
}

// FX55
// Store registers to memory
static void opStoreMemory (uint16_t instruction, chip8 emulator)
{
    uint8_t numRegisters = INSTRUCTION_GET_X (instruction);

    for (int i = 0; i <= numRegisters; i++)
    {
        emulator->memory [emulator->I + i] = emulator->V [i];
    }

    // In the old behaviour, the I register was incremented as it worked.
    // We can simulate this in O(1) by just calculating the new address
    if (crisp8ConfigGetStoreLoadMemory (emulator) == OLD)
    {
        emulator->I += numRegisters + 1;
    }
}

// FX65
// Load registers from memory
static void opLoadMemory (uint16_t instruction, chip8 emulator)
{
    uint8_t numRegisters = INSTRUCTION_GET_X (instruction);

    for (int i = 0; i <= numRegisters; i++)
    {
        emulator->V [i] = emulator->memory [emulator->I + i];
    }

    // In the old behaviour, the I register was incremented as it worked.
    // We can simulate afterwards by just calculating the new address
    if (crisp8ConfigGetStoreLoadMemory (emulator) == OLD)
    {
        emulator->I += numRegisters + 1;
    }
}

// Instruction decoding ------------------------------------------------
// This group of functions decode instructions before executing them.
// They all take the instruction to decode and the emulator to operate on as parameters.

static void decodeType0 (uint16_t instruction, chip8 emulator)
{
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
        case 6:
            opShiftRight (instruction, emulator);
            break;
        case 7:
            opSubVX (instruction, emulator);
            break;
        case 0xE:
            opShiftLeft (instruction, emulator);
            break;
    }
}

static void decodeTypeE (uint16_t instruction, chip8 emulator)
{
    // The instructions in this group are differentiated by the last nibble
    switch (INSTRUCTION_GET_NIBBLE (instruction, 3))
    {
        case 1:
            opSkipIfNotKey (instruction, emulator);
            break;
        case 0xE:
            opSkipIfKey (instruction, emulator);
            break;
    }
}

static void decodeTypeF (uint16_t instruction, chip8 emulator)
{
    // This group of instructions are differentiated by the two last nibbles
    switch (INSTRUCTION_GET_NN (instruction))
    {
        case 0x07:
            opSetVXDelay (instruction, emulator);
            break;
        case 0x15:
            opSetDelayTimer (instruction, emulator);
            break;
        case 0x18:
            opSetSoundTimer (instruction, emulator);
            break;
        case 0x1E:
            opAddToIndex (instruction, emulator);
            break;
        case 0x0A:
            opGetKey (instruction, emulator);
            break;
        case 0x29:
            opFontCharacter (instruction, emulator);
            break;
        case 0x33:
            opDecimalConvert (instruction, emulator);
            break;
        case 0x55:
            opStoreMemory (instruction, emulator);
            break;
        case 0x65:
            opLoadMemory (instruction, emulator);
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
            opJumpWithOffset (instruction, emulator);
            break;
        case 0xC:
            opRandom (instruction, emulator);
            break;
        case 0xD:
            opDraw (instruction, emulator);
            break;
        case 0xE:
            decodeTypeE (instruction, emulator);
            break;
        case 0xF:
            decodeTypeF (instruction, emulator);
            break;
    }
}
