// This is the public API part of crisp8. Use this in your frontend (assuming you want to write a debugger)
#ifndef CRISP_8_STACK_H
#define CRISP_8_STACK_H

#include <stdint.h>

typedef struct chip8Stack_s* chip8Stack;

// Used by crisp8 ------------------------------------------------------

// Performs neccesary initialization of the stack. This must be the first operation performed on a new stack
//
// Parameters:
//  - stack: a pointer to the stack to be initialized
void crisp8StackInit (chip8Stack* stack);

// Frees all memory accociated with the stack
//
// Parameters:
//  - stack: a pointer to the stack to destroy
void crisp8StackDestroy (chip8Stack* stack);

// Pushes a 16 bit unsigned integer onto the stack.
// If this would cause a stack overflow, nothing is pushed and a negative value is returned
//
// Parameters:
//  - stack: the stack to operate on
//  - item: the item to push
//
// Return value:
//  Negative if a stack overflow would occur
int8_t crisp8StackPush (chip8Stack stack, uint16_t item);

// Pops a 16 bit unsigned integer off the stack
// If this would cause a stack underflow, nothing is popped and a negative value is returned
//
// Parameters:
//  - stack: the stack to operate on
//  - item: a variable to put the popped item into
//
// Return value:
//  Negative if a stack underflow would occur
int8_t crisp8StackPop (chip8Stack stack, uint16_t* item);

// For writing debuggers (and a bit internally in the stack) -----------

// Returns the size of the stack
//
// Parameters:
//  - stack: the stack to operate on
//
// Return value:
//  The size of the stack
uint16_t crisp8StackGetSize (chip8Stack stack);

// Returns a pointer to the base of the stack
//
// Parameters:
//  - stack: the stack to operate on
//
// Return value:
//  A pointer to the base of the stack
uint16_t* crisp8StackGetBase (chip8Stack stack);

// Returns a pointer to the current top of the stack (the topmost item, not the base + stack size).
// This pointer will not update as things are pushed on or of the stack, so the function should be called anew every
// time an updated value is desired
//
// Parameters:
//  - stack: the stack to operate on
//
// Return value:
//  A pointer to the top of the stack
uint16_t* crisp8StackGetTop (chip8Stack stack);

// Returns the number of items currently pushed onto the stack
//
// Parameters:
//  - stack: the stack to operate on
//
// Return value:
//  The number of items pushed onto the stack
uint16_t crisp8StackGetNumItems (chip8Stack stack);
#endif
