
#include "config.h"
#include "log.h"

int main ()
{
    Stack_t stack1 = {};
    stack_ctor (stack1, 4);

    for (int index = 0; index < 10; index++)
    {
        stack_push (&stack1, index*index);
    }
    
    
    // stack1.capacity = -1;
    // stack1.data = NULL;

    long long * temp = (long long *) &stack1;

    
    // *temp++ = 0;
    // *temp++ = 0;
    // *temp++ = 0;
    // *temp = 0;
    stack_pop (&stack1);
    
    stack_dump (&stack1, FULL);
    
    stack_dtor (&stack1);
    //stack_dump (&stack1, NAME);

}