#ifndef CRISP8_INSTRUCTIONS_H
#define CRISP8_INSTRUCTIONS_H

#include "crisp8.h"

// Fetches the instruction currently pointed to by PC and increments PC to point to the next instruction
//
// Parameters:
//  - emulator: the used chip-8 emulator
//
// Return value:
//  The fetched instruction
uint16_t fetchInstruction (chip8 emulator);

// Decodes and executes an instruction
//
// Parameters:
//  - instruction: the instruction to execute
//  - emulator: the used chip-8 emulator
void dispatchInstruction (uint16_t instruction, chip8 emulator);
#endif
