Crisp-8 is a backend of a chip-8 emulator. It handles all the chip-8 instructions while allowing different frontends to be written for drawing and playing sound. Crisp-8 takes an array of bytes as input and exposes functions to execute instructions or provide internal information of the emulated machine. It is up to the frontend to:

- Print to the screen
- Supply callbacks for input and audio
- Do the looping
- Read the program file from disk (the backend only interprets it)

## Usage/API
Look at the files in include/public. Also, you have to call `srand` somewhere in your program for the chip-8's random instruction.

## Examples
Examples of how to use parts of the API can be found in the examples directory.
*Link to reference frontend implementation that unfortunately doesn't exist yet*

## Building
Building crisp8 requires a c compiler and cmake.

### Steps for Linux based operating systems
1. Create a build directory and cd into it
```sh
mkdir build
cd build
```
2. Execute cmake. This is the time to choose which configuration options you want to compile in. Look them up in the options part of CMakeLists.txt.
```sh
cmake .. [-D<OPTION>=<ON|OFF>]
```
3. Execute make
```sh
make
```
4. Optionally execute make install to install the library and headers to wherever you defined CMAKE\_PREFIX\_DIR to be
```sh
make install
```
4. Profit?

## NAQ (Never Asked Questions)
Q: Why is it called crisp-8?

A: Because I had just finished a book by an english author when I decided on the name.

Q: Why do you mix crisp8 and chip8 in the source code?

A: All references to the program are written as crisp8 (such as function name prefixes). References to the chip-8 machine are written as chip8 (such as the chip8 type). Yes this was a bad idea, and yes I have probably mixed them up in the code causing lots of confusion. You live and you learn.

## License
GPL3
