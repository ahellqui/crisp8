Crisp-8 is a backend of a chip-8 emulator. It handles all the chip-8 instructions while allowing different frontends to be written for drawing and playing sound. Crisp-8 takes an array of bytes as input and exposes functions to execute instructions or provide internal information of the emulated machine. It is up to the frontend to:

- Print to the screen
- Supply callbacks for input and audio
- Do the looping
- Read the program file from disk (the backend only interprets it)

## Usage/API
Look at crisp8/crisp8.h

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
4. Profit?

## NAQ (Never Asked Questions)
- Q: Why is it called crisp-8?
  A: Because I had just finished a book by an english author when I decided on the name.

- Q: Why do you mix crisp8 and chip8 in the source code?
  A: All references to the program are written as crisp8 (such as function name prefixes). References to the chip-8 machine are written as chip8 (such as the chip8 type).

## License
*To be decided*
