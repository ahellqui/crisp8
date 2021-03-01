#include "crisp8.h"

#include "stack.h"

#define MEMORY_SIZE 4096

#define DISPLAY_WIDTH 64
#define DISPLAY_HEIGHT 32

typedef void (*audioCallback) (void);
typedef uint16_t (*inputCallback) (void);

struct chip8_s
{
    // Memory ------------------------

    // I guess these allocations could pose problems on embedded systems
    uint8_t memory [MEMORY_SIZE];
    uint8_t display [DISPLAY_WIDTH * DISPLAY_HEIGHT];

    chip8Stack stack;

    // Registers ---------------------

    uint16_t PC;
    uint16_t I;
    // Using an array allows us to more easily choose registers from opcodes
    uint8_t V [16];

    // Timers ------------------------

    uint8_t delay_timer;
    uint8_t sound_timer;

    // Non emulator information ------

    // Callbacks
    audioCallback audioCb;
    inputCallback inputCb;

    // Framerate
    uint16_t framerate;
};
