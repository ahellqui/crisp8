#include "config.h"
#include "crisp8_private.h"

void crisp8ConfigSetShift (enum crisp8ConfigValue value, chip8 emulator)
{
    emulator->config.instructionShift = value;
}

enum crisp8ConfigValue crisp8ConfigGetShift (chip8 emulator)
{
    return emulator->config.instructionShift;
}

void crisp8ConfigSetJumpOffset (enum crisp8ConfigValue value, chip8 emulator)
{
    emulator->config.instructionJumpOffset = value;
}

enum crisp8ConfigValue crisp8ConfigGetJumpOffset (chip8 emulator)
{
    return emulator->config.instructionJumpOffset;
}
