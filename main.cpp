
#include "config.h"
#include "log.h"
#include <math.h>
int main ()
{
    Stack_t stack1 = {};
    stack_ctor (stack1, 2);

    for (int index = 0; index < 6; index++)
    {
        stack_push (&stack1, index*index);
    }

    //stack1.data[0] = 10;
    //stack1.left_canary = 1000;         
    
    // long long * t = (long long *) &stack1.data;
    // *t++ = 0;
    // *t++ = 0;



    stack_pop (&stack1);

    stack_dump (&stack1, FULL);
    
    stack_dtor (&stack1);
}