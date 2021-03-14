// These configuration options are for certain ambiguous instructions where a frontend might want to choose behaviour
#ifndef CRISP8_CONFIG_H
#define CRISP8_CONFIG_H

#include "crisp8.h"

#include <stdbool.h>

// The definition of the configuration struct used in the emulator.
// Its members are the available confguration options set by functions exposed in this header.
struct crisp8Config
{
    bool useInstructionShiftNew;
};

// Use the new behaviour in shift instructions (put VY in VX before shifting)
//
// Parameters:
//  - value: the value to set the confguration option to (true or false)
//  - emulator: the used chip-8 emulator
void crisp8ConfigSetShiftNew (bool value, chip8 emulator);

// Get the value of the configuration option useInstructionShiftNew
bool crisp8ConfigGetShiftNew (chip8 emulator);

#endif
