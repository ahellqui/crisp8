// Definition of the chip-8 struct
#ifndef CRISP8_PRIVATE_H
#define CRISP8_PRIVATE_H

#include <stdint.h>
#include <stdbool.h>

#include "defs.h"
#include "stack.h"

typedef void (*crisp8AudioCallback) (void);
typedef uint16_t (*crisp8InputCallback) (void);

struct chip8_s
{
    // Memory ------------------------

    // I guess these allocations could pose problems on embedded systems
    uint8_t memory [CRISP8_MEMORY_SIZE];
    uint8_t display [CRISP8_DISPLAY_WIDTH * CRISP8_DISPLAY_HEIGHT];

    chip8Stack stack;

    // Registers ---------------------

    uint16_t PC;
    uint16_t I;
    // Using an array allows us to more easily choose registers from opcodes
    uint8_t V [16];

    // Timers ------------------------

    uint8_t delayTimer;
    uint8_t soundTimer;

    // Non emulator information ------

    // Timer remainder counters (used to make sure they decrement at 60hz)
    float delayTimerRemainder;
    float soundTimerRemainder;

    // Sound timer state
    bool soundPlaying;

    // Callbacks
    crisp8AudioCallback audioCb;
    crisp8InputCallback inputCb;

    // Framerate
    uint16_t framerate;
};
#endif
