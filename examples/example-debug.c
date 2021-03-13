#include "../include/public/crisp8.h"

#include <stdio.h>

int main ()
{
    chip8 emulator;
    crisp8Init (&emulator);

    struct crisp8Debug debugStruct;
    crisp8InitDebugStruct (&debugStruct, emulator);

    // A completely useless program that changes a few registers just to show that you can read and write them with
    // the crisp8Debug struct
    uint8_t program [] = {0x10, 0x00,   // Jump to address 0
                          0xA2, 0x55,   // Set index register to 0x255
                          0x64, 0x0F,   // Set register V4 to 0x0F
                          0x74, 0x01,   // Add 0x01 to register V4
    };

    crisp8InitializeProgram (emulator, program, sizeof (program) / sizeof (program [0]));

    // First instruction
    printf ("PC before: %d\n", *debugStruct.PC);
    crisp8RunCycle (emulator);
    printf ("PC after: %d\n\n", *debugStruct.PC);

    // Move back to where we were supposed to be in the program before we changed PC (this also demonstrates that you
    // can set values)
    *debugStruct.PC = 0x202;

    // Second instruction
    printf ("I before: 0x%X\n", *debugStruct.I);
    crisp8RunCycle (emulator);
    printf ("I after: 0x%X\n\n", *debugStruct.I);

    // Third instruction
    printf ("V4 before: 0x%X\n", debugStruct.V [0x4]);
    crisp8RunCycle (emulator);
    printf ("V4 after: 0x%X\n\n", debugStruct.V [0x4]);

    // Fourth instruction
    printf ("V4 before: 0x%X\n", debugStruct.V [0x4]);
    crisp8RunCycle (emulator);
    printf ("V4 after: 0x%X\n\n", debugStruct.V [0x4]);

    crisp8Destroy (&emulator);
}
