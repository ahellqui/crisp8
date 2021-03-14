#include "config.h"
#include "crisp8_private.h"

void crisp8ConfigSetShiftNew (bool value, chip8 emulator)
{
    emulator->config.useInstructionShiftNew = value;
}

bool crisp8ConfigGetShiftNew (chip8 emulator)
{
    return emulator->config.useInstructionShiftNew;
}
