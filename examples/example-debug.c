// I kind of made this example to test some opcodes, but it demonstrates the debug interface as well
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
                          0x65, 0x9C,   // Set register V5 to 0x9C (156)
                          0xF5, 0x33,   // Write the decimal value of register V5 to memory at index register
                          0xF4, 0x1E,   // Add V4 to index register
                            
                          // Set up for the store instruction
                          0x60, 0x05,   // Set register V0 to 0x05
                          0x61, 0x08,   // Set register V1 to 0x08
                          0x62, 0x14,   // Set register V1 to 0x14
                          0x63, 0x2B,   // Set register V1 to 0x14

                          // Store to memory
                          0xF3, 0x55,

                          // Set up for the load instruction
                          0x60, 0x00,   // Set register V0 to 0x05
                          0x61, 0x00,   // Set register V1 to 0x08
                          0x62, 0x00,   // Set register V1 to 0x14
                          0x63, 0x00,   // Set register V1 to 0x14

                          // Store from memory
                          0xF3, 0x65,

                          // Load address of font character A to index register
                          0x66, 0x0A,   // Set register V6 to 0x0A
                          0xF6, 0x29,   // Set the index register to the address of the font for the value in V6 (A)
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

    // Fifth instruction (no need to test)
    crisp8RunCycle (emulator);

    // Sixth instruction
    printf ("Memory before: I, I+1, I+2: 0x%X 0x%X 0X%X\n", debugStruct.memory [*debugStruct.I],
                                                            debugStruct.memory [*debugStruct.I + 1],
                                                            debugStruct.memory [*debugStruct.I + 2]);
    crisp8RunCycle (emulator);
    printf ("Memory after: I, I+1, I+2: 0x%X 0x%X 0X%X\n\n", debugStruct.memory [*debugStruct.I],
                                                             debugStruct.memory [*debugStruct.I + 1],
                                                             debugStruct.memory [*debugStruct.I + 2]);

    // Seventh instruction:
    printf ("I before: 0x%X, V4 before: 0x%X\n", *debugStruct.I, debugStruct.V [4]);
    crisp8RunCycle (emulator);
    printf ("I after: 0x%X\n\n", *debugStruct.I);

    // Instruction 8, 9, 10, 11:
    crisp8RunCycle (emulator);
    crisp8RunCycle (emulator);
    crisp8RunCycle (emulator);
    crisp8RunCycle (emulator);
    printf ("V0, V1, V2, V3: 0x%X 0x%X 0x%X 0x%X\n\n", debugStruct.V [0], debugStruct.V [1],
                                                       debugStruct.V [2], debugStruct.V [3]);

    // Instruction 12
    printf ("Memory before: I, I+1, I+2, I+3: 0x%X 0x%X 0X%X 0x%X\n", debugStruct.memory [*debugStruct.I],
                                                                      debugStruct.memory [*debugStruct.I + 1],
                                                                      debugStruct.memory [*debugStruct.I + 2],
                                                                      debugStruct.memory [*debugStruct.I + 3]);
    crisp8RunCycle (emulator);
    printf ("Memory after: I, I+1, I+2, I+3: 0x%X 0x%X 0X%X 0x%X\n\n", debugStruct.memory [*debugStruct.I],
                                                                       debugStruct.memory [*debugStruct.I + 1],
                                                                       debugStruct.memory [*debugStruct.I + 2],
                                                                       debugStruct.memory [*debugStruct.I + 3]);

    // Instruction 13, 14, 15, 16:
    crisp8RunCycle (emulator);
    crisp8RunCycle (emulator);
    crisp8RunCycle (emulator);
    crisp8RunCycle (emulator);

    // Instruction 17
    printf ("Registers before: V0, V1, V2, V3: 0x%X 0x%X 0x%X 0x%X\n", debugStruct.V [0], debugStruct.V [1],
                                                                         debugStruct.V [2], debugStruct.V [3]);
    crisp8RunCycle (emulator);
    printf ("Registers after: V0, V1, V2, V3: 0x%X 0x%X 0x%X 0x%X\n\n", debugStruct.V [0], debugStruct.V [1],
                                                                        debugStruct.V [2], debugStruct.V [3]);

    // Instruction 18, 19
    crisp8RunCycle (emulator);
    crisp8RunCycle (emulator);
    printf ("I: 0x%X\n\n", *debugStruct.I);

    crisp8Destroy (&emulator);
}
