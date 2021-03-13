// This program demonstrates the use of the internal stack of the chip-8 emulator in crisp8. It is probably only
// useful for you if you want to create a debugger, as the whole point of crisp8 is that you don't have to be concerned
// with this stuff

#include "../include/public/stack.h"

#include <stdio.h>

int main (void)
{
    chip8Stack stack;
    uint16_t popped;

    // Initialize
    crisp8StackInit (&stack);

    // Get base and top of stack
    printf ("Base: %p\n", crisp8StackGetBase (stack));
    printf ("Top : %p\n", crisp8StackGetTop (stack));

    // Get stack size
    uint16_t stack_size = crisp8StackGetSize (stack);
    printf ("Stack size: %d\n", stack_size);

    // Push single item
    crisp8StackPush (stack, 15);

    // Pop single item
    crisp8StackPop (stack, &popped);
    printf ("Popped: %d\n", popped);

    // Stack underflow causes a negative return value
    printf ("%d\n\n", crisp8StackPop (stack, &popped));

    // Stack overflow causes a negative return value
    for (int i = 0; i < stack_size; i++)
    {
        if (crisp8StackPush (stack, i) < 0)
        {
            puts ("Stack overflow");
        }
        else
        {
            printf ("Pushed: %d\n", i);
        }
    }

    puts ("");
    printf ("Number of items in stack: %d\n", crisp8StackGetNumItems (stack));
    puts ("");

    for (int i = 0; i < stack_size + 1; i++)
    {
        if (crisp8StackPop (stack, &popped) < 0)
        {
            puts ("Stack underflow");
        }
        else
        {
            printf ("Popped: %d\n", popped);
        }
    }

    crisp8StackDestroy (&stack);
    return 0;
}
