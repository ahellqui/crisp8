// Definition of the chip-8 struct
#ifndef CRISP8_PRIVATE_H
#define CRISP8_PRIVATE_H

// The font needs to be located anywhere before 0x200, but apparently it's convention to start
// it between 0x50 and 0x9F so I'll follow that
#define CRISP8_FONT_START_ADDRESS 0x50

// Programs are loaded into memory at adress 0x200 since the chip8 iself originally
// took up the first 0x1FF bytes of the host computers memory
#define CRISP8_PROGRAM_START_ADDRESS 0x200

#include <stdint.h>
#include <stdbool.h>

#include "defs.h"
#include "stack.h"
#include "config.h"

typedef void (*crisp8AudioCallback) (void);
typedef uint32_t (*crisp8InputCallback) (void);

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

    // Configuration for some ambiguous instructions
    struct crisp8Config config;

    // Last cycles keystate (used to check for key release)
    uint32_t lastKeyState;
};
#endif
