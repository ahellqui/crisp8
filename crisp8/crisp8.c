#include "crisp8.h"

// I'm doing my best to avoid heap allocations.
#define MEMORY_SIZE 4096

#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32

struct chip8_s
{
    // Memory ------------------------

    // I guess these allocations could pose problems on embedded systems
    uint8_t memory [MEMORY_SIZE];
    uint8_t display [DISPLAY_WIDTH * DISPLAY_HEIGHT];

    // Stack to be added

    // Registers ---------------------

    uint8_t PC;
    uint16_t I;
    // Using an array allows us to more easily choose registers from opcodes
    uint8_t V [16];

    // Timers ------------------------

    uint8_t delay_timer;
    uint8_t sound_timer;

    // Callbacks to be added
};
