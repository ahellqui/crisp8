#include "crisp8.h"
#include "stack.h"
#include "defs.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

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

    uint8_t delayTimer;
    uint8_t soundTimer;

    // Non emulator information ------

    // Timer remainder counters
    float delayTimerRemainder;
    float soundTimerRemainder;

    // Sound timer state
    bool soundPlaying;

    // Callbacks
    audioCallback audioCb;
    inputCallback inputCb;

    // Framerate
    uint16_t framerate;
};

// Loads the font of the chip-8 into its memory
static void loadFont (chip8 emulator)
{
    // This is a commonly used font. I might design my own in the future
    const uint8_t font [] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0,   // 0
        0x20, 0x60, 0x20, 0x20, 0x70,   // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0,   // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, 	// 3
        0x90, 0x90, 0xF0, 0x10, 0x10, 	// 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, 	// 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, 	// 6
        0xF0, 0x10, 0x20, 0x40, 0x40, 	// 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, 	// 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, 	// 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, 	// A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, 	// B
        0xF0, 0x80, 0x80, 0x80, 0xF0, 	// C
        0xE0, 0x90, 0x90, 0x90, 0xE0, 	// D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, 	// E
        0xF0, 0x80, 0xF0, 0x80, 0x80  	// F
    };

    memcpy (emulator->memory + CRISP8_FONT_START_ADDRESS, font, sizeof (font));
}

static void decrementDelayTimer (chip8 emulator)
{
    if (emulator->delayTimer == 0)
    {
        return;
    }

    emulator->delayTimerRemainder += (60.0f / emulator->framerate);

    if (emulator->delayTimerRemainder > 1)
    {
        if (emulator->delayTimer < (uint8_t)emulator->delayTimerRemainder)
        {
            emulator->delayTimer = 0;
        }
        else
        {
            emulator->delayTimer -= (uint8_t)emulator->delayTimerRemainder;
        }

        emulator->delayTimerRemainder -= (uint8_t)emulator->delayTimerRemainder;
    }
}

static void decrementSoundTimer (chip8 emulator)
{
    if (emulator->soundTimer == 0)
    {
        return;
    }

    emulator->soundTimerRemainder += (60.0f / emulator->framerate);

    if (emulator->soundTimerRemainder > 1)
    {
        if (emulator->soundTimer < (uint8_t)emulator->soundTimerRemainder)
        {
            emulator->soundTimer = 0;
        }
        else
        {
            emulator->soundTimer -= (uint8_t)emulator->soundTimerRemainder;
        }

        emulator->soundTimerRemainder -= (uint8_t)emulator->soundTimerRemainder;
    }
}

static void decrementTimers (chip8 emulator)
{
    decrementDelayTimer (emulator);
    decrementSoundTimer (emulator);
}

static void playSound (chip8 emulator)
{
    if (emulator->soundTimer > 0)
    {
        if (emulator->soundPlaying == true)
        {
            return;
        }

        emulator->audioCb ();
        emulator->soundPlaying = true;
    }
    else if (emulator->soundPlaying == true)
    {
        emulator->audioCb ();
        emulator->soundPlaying = false;
    }
}

void crisp8Init (chip8* emulator)
{
    *emulator = malloc (sizeof (**emulator));
    if (!(*emulator))
    {
        fputs ("Out of memory in crisp8Init; aborting", stderr);
        abort ();
    }

    crisp8StackInit (&(*emulator)->stack);

    loadFont (*emulator);
}

void crisp8Destroy (chip8* emulator)
{
    crisp8StackDestroy (&(*emulator)->stack);
    free (*emulator);
    *emulator = NULL;
}

void crisp8SetFramerate (chip8 emulator, uint16_t framerate)
{
    emulator->framerate = framerate;
}

void crisp8SetAudioCallback (chip8 emulator, audioCallback callback)
{
    emulator->audioCb = callback;
}

void crisp8SetInputCallback (chip8 emulator, inputCallback callback)
{
    emulator->inputCb = callback;
}

void crisp8InitializeProgram (chip8 emulator, uint8_t* program, uint16_t program_size)
{
    memcpy (emulator->memory + CRISP8_PROGRAM_START_ADDRESS, program, program_size);
    emulator->PC = CRISP8_PROGRAM_START_ADDRESS;
}
}

const uint8_t* const crisp8GetFramebuffer (chip8 emulator)
{
    return emulator->display;
}
