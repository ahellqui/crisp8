// Useful definitions
#ifndef CRISP8_DEFINES_H
#define CRISP8_DEFINES_H

// The RAM size of the chip-8 in bytes
#define CRISP8_MEMORY_SIZE 4096

// The width and height of the chip-8's display
#define CRISP8_DISPLAY_WIDTH 64
#define CRISP8_DISPLAY_HEIGHT 32

// Helper macros to write your input callback
#define CRISP8_KEYPAD_0 (1 << 0x0)
#define CRISP8_KEYPAD_1 (1 << 0x1)
#define CRISP8_KEYPAD_2 (1 << 0x2)
#define CRISP8_KEYPAD_3 (1 << 0x3)
#define CRISP8_KEYPAD_4 (1 << 0x4)
#define CRISP8_KEYPAD_5 (1 << 0x5)
#define CRISP8_KEYPAD_6 (1 << 0x6)
#define CRISP8_KEYPAD_7 (1 << 0x7)
#define CRISP8_KEYPAD_8 (1 << 0x8)
#define CRISP8_KEYPAD_9 (1 << 0x9)
#define CRISP8_KEYPAD_A (1 << 0xA)
#define CRISP8_KEYPAD_B (1 << 0xB)
#define CRISP8_KEYPAD_C (1 << 0xC)
#define CRISP8_KEYPAD_D (1 << 0xD)
#define CRISP8_KEYPAD_E (1 << 0xE)
#define CRISP8_KEYPAD_F (1 << 0xF)

#endif
