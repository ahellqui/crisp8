// These configuration options are for certain ambiguous instructions where a frontend might want to choose behaviour
#ifndef CRISP8_CONFIG_H
#define CRISP8_CONFIG_H

#include "crisp8.h"

#include <stdbool.h>

enum crisp8ConfigValue { OLD, NEW };

// The definition of the configuration struct used in the emulator.
// Its members are the available confguration options set by functions exposed in this header.
struct crisp8Config
{
    // In the new behaviour, the value in VY is put in VX before shifting.
    enum crisp8ConfigValue instructionShift;
};

// Use the new behaviour in shift instructions (put VY in VX before shifting)
//
// Parameters:
//  - value: the value to set the confguration option to (OLD or NEW)
//  - emulator: the used chip-8 emulator
void crisp8ConfigSetShift (enum crisp8ConfigValue value, chip8 emulator);

// Get the value of the configuration option useInstructionShiftNew
enum crisp8ConfigValue crisp8ConfigGetShift (chip8 emulator);

#endif
