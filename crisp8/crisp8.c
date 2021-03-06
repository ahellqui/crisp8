#include "crisp8_private.h"
#include "crisp8.h"
#include "instructions.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

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

// Decrements the delay timer by the appropriate amount if it is greater than 0
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

// Decrements the sound timer by the appropriate amount if it is greater than 0
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

#ifdef CRISP8_DISPLAY_USE_ALPHA
static void decrementDisplayAlpha (chip8 emulator)
{
    for (int i = 0; i < CRISP8_DISPLAY_WIDTH * CRISP8_DISPLAY_HEIGHT; i++)
    {
        // A pixel is deemed of if is not at full brightness. This just slowly decrements it.
        if (emulator->display [i] < 0xFF)
        {
            emulator->display [i] -= 1;
        }
    }
}
#endif

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

    memset (*emulator, 0, sizeof (**emulator));

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

void crisp8SetAudioCallback (chip8 emulator, crisp8AudioCallback callback)
{
    emulator->audioCb = callback;
}

void crisp8SetInputCallback (chip8 emulator, crisp8InputCallback callback)
{
    emulator->inputCb = callback;
}

void crisp8InitializeProgram (chip8 emulator, uint8_t* program, uint16_t program_size)
{
    memcpy (emulator->memory + CRISP8_PROGRAM_START_ADDRESS, program, program_size);
    emulator->PC = CRISP8_PROGRAM_START_ADDRESS;
}

void crisp8RunCycle (chip8 emulator)
{
    // Timer business
    decrementTimers (emulator);
    playSound (emulator);

    // Decrement the display alpha values if compiled with those
#ifdef CRISP8_DISPLAY_USE_ALPHA
    decrementDisplayAlpha (emulator);
#endif

    // Fetch
    uint16_t instruction = fetchInstruction (emulator);
    // Decode and execute
    dispatchInstruction (instruction, emulator);
}

const uint8_t* const crisp8GetFramebuffer (chip8 emulator)
{
    return emulator->display;
}
