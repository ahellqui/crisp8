// This is the public API part of crisp8. Use this in your frontend
#ifndef CRISP8_H
#define CRISP8_H

#include <stdint.h>

#include "stack.h"

typedef struct chip8_s* chip8;

// Initialization and deinitialization ---------------------------------

// Performs neccesary initialization of the chip-8 emulator. This must be the first operation performed on a new chip8
//
// Parameters:
//  - emulator: a pointer to the emulator being initialized
void crisp8Init (chip8* emulator);

// Frees all memory accociated with the emulator. This must be the last function it's used in
//
// Parameters:
//  - emulator: a pointer to the emulator to destroy
void crisp8Destroy (chip8* emulator);

// The chip-8-backend isn't responsible for any sort of looping, however it needs to know the framerate its running at
// to properly function.
//
// Parameters:
//  - emulator: the used chip-8 emulator
//  - framerate: the framerate it's running at in frames per second
void crisp8SetFramerate (chip8 emulator, uint16_t framerate);

// Because audio support will vary between platforms, all frontends have to supply a callback that plays sound.
// The callback should toggle a sound (a beep is suggested, but you do you..).
//
// Parameters:
//  - emulator: the used chip-8 emulator
//  - callback: a function pointer to the callback function
void crisp8SetAudioCallback (chip8 emulator, void (*callback) (void));

// Because input support will vary between platforms, all frontends have to supply a callback that returns the state of
// the keypad.
//
// The callback should return a 32 bit bitmask containing the state of all 16 keys on the keypad. The keys are
// represented in the following order from least to most significant bit: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, A, B, C, D, E, F.
// Macros to help with this are provided in defs.h
//
// Parameters:
//  - emulator: the used chip-8 emulator
//  - callback: a function pointer to the callback function
void crisp8SetInputCallback (chip8 emulator, uint32_t (*callback) (void));

// The chip-8 program resides completely in memory. The frontend is respnsible for doing the file io to read in the
// program, which is then passed into the backend in the form of an array.
//
// Parameters:
//  - emulator: the used chip-8 emulator
//  - program: an array of the chip-8 program in raw bytes
//  - program_size: the size of program
void crisp8InitializeProgram (chip8 emulator, uint8_t* program, uint16_t program_size);

// Program execution ---------------------------------------------------

// Execute a single cpu cycle/instruction. This is the function you probably want to call every loop iteration
//
// Parameters:
//  - emulator: the used chip-8 emulator
void crisp8RunCycle (chip8 emulator);

// Returns a pointer to the framebuffer for the frontend to draw to the screen. The returned pointer is technically
// r/w because we don't want to copy memory, but it should be treated as read only. The framebuffer is 64x32 pixels
// long, each pixel represented by an 8 bit integer. If the program is compiled with CRISP8_DISPLAY_USE_ALPHA defined,
// the integers value may be treated as an alpha value for an "old monitor fading effect". Otherwise a value of zero
// means off and 1 means on.
//
// Parameters:
//  - emulator: the used chip-8 emulator
//
// Return value:
//  A pointer to the emulators framebuffer
const uint8_t* const crisp8GetFramebuffer (chip8 emulator);

// Debugging -----------------------------------------------------------

// A struct containing pointers to the chip-8 emulators memory, stack and registers.
// The members are direct pointers to the chip8's members, so they will update every cycle.
// You may read or write any of it's members (except for the stack) directly, but be careful to not cause segmentation
// faults.
//
// The stack has to be interacted with through the API defined in stack.h
struct crisp8Debug
{
    uint8_t* memory;
    chip8Stack stack;

    uint16_t* PC;
    uint16_t* I;

    //Use V [0xN] to access register N
    uint8_t* V;
};

// Initializes a crisp8Debug struct with the addresses of emulator's members.
// This only needs to be called once since the members will update every chip-8 cycle.
//
// Parameters:
//  - debugStruct: The crisp8Debug struct to initialize
//  - emulator: The chip8 which members will be used to initialize debugStruct
void crisp8InitDebugStruct (struct crisp8Debug* debugStruct, chip8 emulator);
#endif
