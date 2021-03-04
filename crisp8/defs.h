#ifndef CRISP8_DEFINES
#define CRISP8_DEFINES 


// The font needs to be located anywhere before 0x200, but apparently it's convention to start
// it between 0x50 and 0x9F so I'll follow that
#define CRISP8_FONT_START_ADDRESS 0x50

// Programs are loaded into memory at adress 0x200 since the chip8 iself originally
// took up the first 0x1FF bytes of the host computers memory
#define CRISP8_PROGRAM_START_ADDRESS 0x200

#endif
