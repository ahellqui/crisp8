#include "stack.h"

#include <stdlib.h>
#include <stdio.h>

// This size is viable according to someone on the internet so surely it has to be true
#define STACK_SIZE 16

struct chip8Stack_s
{
    uint16_t stack [STACK_SIZE];
    uint16_t* stackPtr;
};

// Increments the stack pointer.
//
// Parameters:
//  stack: The stack to operate on
//  amount: the amount to increment by. It should probably always be 1 or -1, but if you know what you're doing it's
//          capable of anything
static void incrementStackPtr (chip8Stack stack, int16_t amount)
{
    stack->stackPtr += amount;
}

// Decrement the stack pointer
//
// Parameters:
//  stack: The stack to operate on
//  amount: the amount to decrement by. It should probably always be 1 or -1, but if you know what you're doing it's
//          capable of anything
static void decrementStackPtr (chip8Stack stack, int16_t amount)
{
    incrementStackPtr (stack, -amount);
}

void crisp8StackInit (chip8Stack* stack)
{
    // I don't want to malloc here, but I simply don't know how I would allocate opaque types on the stack.
    *stack = malloc (sizeof (**stack));
    if (*stack == NULL)
    {
        fputs ("Out of memory in crisp8StackInit; aborting", stderr);
        abort ();
    }

    (*stack)->stackPtr = (*stack)->stack;
}

void crisp8StackDestroy (chip8Stack* stack)
{
    free (*stack);
    *stack = NULL;
}

// Not sure if I should use the getTop and getBase functions or just use the struct members directly.
// The advantage of doing it like this is that I can change the struct. The disadvantage is that I can't modify the
// pointers
int8_t crisp8StackPush (chip8Stack stack, uint16_t item)
{
    uint16_t* stackPtr = crisp8StackGetTop (stack);
    uint16_t* basePtr = crisp8StackGetBase (stack);

    if (stackPtr - basePtr == STACK_SIZE)
    {
        return -1;
    }

    *stackPtr = item;
    incrementStackPtr (stack, 1);

    return 0;
}

int8_t crisp8StackPop (chip8Stack stack, uint16_t* item)
{
    uint16_t* stackPtr = crisp8StackGetTop (stack);
    uint16_t* basePtr = crisp8StackGetBase (stack);

    if (stackPtr - 1 < basePtr)
    {
        return -1;
    }

    *item = *(stackPtr - 1);
    decrementStackPtr (stack, 1);

    return 0;
}

uint16_t crisp8StackGetSize (chip8Stack stack)
{
    return STACK_SIZE;
}

uint16_t* crisp8StackGetTop (chip8Stack stack)
{
    return stack->stackPtr;
}

uint16_t* crisp8StackGetBase (chip8Stack stack)
{
    return stack->stack;
}

uint16_t crisp8StackGetNumItems (chip8Stack stack)
{
    uint16_t* stackPtr = crisp8StackGetTop (stack);
    uint16_t* basePtr = crisp8StackGetBase (stack);

    return stackPtr - basePtr;
}
