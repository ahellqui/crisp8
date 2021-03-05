#ifndef CRISP8_DEFINES_H
#define CRISP8_DEFINES_H


// The font needs to be located anywhere before 0x200, but apparently it's convention to start
// it between 0x50 and 0x9F so I'll follow that
#define CRISP8_FONT_START_ADDRESS 0x50

// Programs are loaded into memory at adress 0x200 since the chip8 iself originally
// took up the first 0x1FF bytes of the host computers memory
#define CRISP8_PROGRAM_START_ADDRESS 0x200

#define CRISP8_MEMORY_SIZE 4096

#define CRISP8_DISPLAY_WIDTH 64
#define CRISP8_DISPLAY_HEIGHT 32


#endif
