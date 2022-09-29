
#include "check_stack.hpp"
#include "log.h"
//#include "stack_functions.hpp"

FILE *output_file  = NULL;
const char *output_file_name = "output.txt";

int main ()
{
    
    output_file = fopen (output_file_name, "w");

    if (output_file == NULL)
    {
        printf ("open file error");
        return  0;
    }

    Stack_t stack1 ;
    stack_ctor (stack1, 2);
    
    for (int index = 0; index < 6; index++)
    {
        stack_push (&stack1, index*index);
    }

    stack1.capacity = 10;
    //stack1.data[0] = 100;         


    stack_pop (&stack1);

    stack_dump (output_file, &stack1, FULL);

    stack_dtor (&stack1);

}