#include "crisp8.h"
#include "stack.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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

void crisp8Init (chip8* emulator)
{
    *emulator = malloc (sizeof (**emulator));
    if (!(*emulator))
    {
        fputs ("Out of memory in crisp8Init; aborting", stderr);
        abort ();
    }

    crisp8StackInit (&(*emulator)->stack);
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
    // Programs are loaded into memory at adress 0x200 since the chip8 iself originally
    // took up the first 0x1FF bytes of the host computers memory
    memcpy (emulator->memory + 0x200, program, program_size);
    emulator->PC = 0x200;
}
