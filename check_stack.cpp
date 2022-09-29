#include <math.h>
#include "log.h"
#include <assert.h>
#include "check_stack.hpp"



Elem_t * create_data_canary (Stack_t * stack)
{
    assert (stack);

    int * stack_data = (int *) calloc (1, (sizeof (LEFT_DATA_CANARY) + stack->capacity*sizeof(Elem_t) + sizeof (RIGHT_DATA_CANARY)));

    stack_data[0] = LEFT_DATA_CANARY;
    stack_data++;
    stack_data[stack->capacity*sizeof(Elem_t)/sizeof(int)] = RIGHT_DATA_CANARY;
    
    return (Elem_t * ) stack_data;
}


Elem_t * recreate_data_canary (Stack_t * stack)
{
    assert (stack);
    
    int * stack_data = (int *) stack->data;

    int temp_canary = stack_data[stack->capacity/multiple*sizeof (Elem_t)/sizeof(int)];
    
    stack_data--;

    stack_data = (int*) recalloc (stack_data, sizeof (LEFT_DATA_CANARY) + stack->capacity*sizeof (Elem_t) + sizeof (RIGHT_DATA_CANARY), 1);

    stack_data++;

    stack_data[stack->capacity*sizeof (Elem_t)/sizeof(int)] = temp_canary;
    
    return (Elem_t *) stack_data;
}


void stack_dump_ (FILE *file, Stack_t *stack, DUMP_MODE mode, const char *func_name, const char *file_name, const int line)
{
    assert (stack);
      
    const char * error = "ok";

    if (stack_ok (stack) != 0)
        error  = "ERROR";

    fprintf (file, "\n%s() at %s.cpp (%d)\n", func_name, file_name, line);
    fprintf (file, "stack[%p] (%s) \"%s\" ", stack, error, stack->stack_info.stack_name);
    fprintf (file, "at %s() at %s (%d)\n", stack->stack_info.func_name, stack->stack_info.file_name, stack->stack_info.line);
    fprintf (file, "{\n size     = %d\n capacity = %d\n\n", stack->size, stack->capacity);

    if (mode == FULL or (stack->capacity <= 6 and mode == SHORT))
        for (unsigned index = 0; index < stack->capacity; index++)
            print_stack_data (file, index, stack->data[index]);
    
    else if (mode == SHORT)
    {
        for (unsigned index = 0; index < 3; index++) 
            print_stack_data (file, index, stack->data[index]);
        
        fprintf (file,"\n........\n\n");
        
        for (unsigned index = stack->capacity - 3; index < stack->capacity; index++) 
            print_stack_data (file, index, stack->data[index]);
    }
    
    fprintf (file,"}\n\n");
}



void print_stack_data (FILE *file, int index, int stack_data)
{
    if (stack_data != int_poison)
        fprintf (file, "*[%d] = %d\n", index, stack_data);
    else
        fprintf (file, " [%d] = %X (poison)\n", index, stack_data);
}


void print_stack_data (FILE *file, int index, double stack_data)
{
    if (!isnan (stack_data))
        fprintf (file, "*[%d] = %lg\n", index, stack_data);
    else
        fprintf (file, " [%d] = NAN (poison)\n", index);
}



unsigned long stack_ok (Stack_t *stack)
{
    assert (stack);
    
    unsigned long error = 0;

    if  ((int) stack->size > (int) stack->capacity)
        error = error | (1 << CAPACITY_SIZE_ERROR);
    
    if (stack->data == NULL)
        error = error | (1 << DATA_POINTER_ERROR);
    #ifdef CANARYCHECK
    else
        error = error | (data_canary_ok (stack) << LEFT_DATA_CANARY_ERROR);
    #endif

    if (((int) stack->capacity < 0))
        error = error | (1 << ZERO_CAPACITY_ERROR);

    #ifdef HASHCHECK
    if (!error )error = error | (check_stack_hash (stack) << HASH_ERROR);
    #endif

    #ifdef CANARYCHECK
    error = error | (canary_ok (stack) << LEFT_CANARY_ERROR);
    #endif

    return error;
}


void calculate_stack_hash (Stack_t * stack)
{
    assert (stack);

    stack->hash = 0;
    stack->hash = calculate_hash (&stack->left_canary, sizeof (stack));

    stack->hash += calculate_hash (stack->data - sizeof (LEFT_DATA_CANARY) , sizeof (LEFT_DATA_CANARY) +
                                                                             stack->capacity*sizeof(Elem_t) + 
                                                                             sizeof (RIGHT_DATA_CANARY));

    stack->hash += calculate_hash ((char *) &stack->hash, sizeof(stack->hash));
}


int  calculate_hash (void * obj, size_t size)
{
    int hash = 0;
    unsigned char * point = (unsigned char *) obj;

    while (size--)
    {
        //printf ("%d ", hash);
        //hash  *= 7;
        //printf ("%d \n\n", hash);
        hash += (point[size] << 5 | 1);
    }
    return hash;
}


bool check_stack_hash (Stack_t *stack)
{
    assert (stack);

    if (stack->hash == update_stack_hash (stack))
        return 0;
    
    return 1;
}


int update_stack_hash (Stack_t *stack)
{    
    assert (stack);

    int stack_hash = calculate_hash (&stack->left_canary, sizeof (stack));
    stack_hash    += calculate_hash (stack->data - sizeof (LEFT_DATA_CANARY) , sizeof (LEFT_DATA_CANARY) +
                                                                               stack->capacity*sizeof(Elem_t) + 
                                                                               sizeof (RIGHT_DATA_CANARY));
    
    
    stack_hash += calculate_hash ((char *) &stack_hash, sizeof(stack_hash));

    return stack_hash;
}


void print_stack_error (unsigned long errors_code)
{
    if (errors_code >> CAPACITY_SIZE_ERROR & 1)
        fprintf (output_file, "WRONG CAPACITY\n");
    
    if (errors_code >> DATA_POINTER_ERROR & 1)
        fprintf (output_file, "DATA POINTER TO NULL\n");
    
    if (errors_code >> ZERO_CAPACITY_ERROR & 1)
        fprintf (output_file, "ZERO CAPACITY\n");
    
    if (errors_code >> LEFT_CANARY_ERROR & 1)
        fprintf (output_file, "LEFT CANARY ERROR\n");
    if (errors_code >> RIGHT_CANARY_ERROR & 1)
        fprintf (output_file, "RIGHT CANARY ERROR\n");
    
    if (errors_code >> LEFT_DATA_CANARY_ERROR & 1)
        fprintf (output_file, "LEFT DATA CANARY ERROR\n");
    
    if (errors_code >> RIGHT_DATA_CANARY_ERROR & 1)
        fprintf (output_file, "RIGHT DATA CANARY ERROR\n");
    
    #ifdef HASHCHECK
    if (errors_code >> HASH_ERROR & 1)
        fprintf (output_file, "HASH ERROR\n");
    #endif 
}

unsigned short canary_ok (Stack_t *stack)
{
    unsigned short error = 0;
    if (stack->left_canary != LEFT_CANARY)
        error = error | 1;
    else if (stack->right_canary != RIGHT_CANARY)
        error = error | 2;

    return error;

}

unsigned short data_canary_ok (Stack_t *stack)
{
    assert (stack);

    unsigned short error = 0;

    if (((int *)stack->data)[-1] != LEFT_DATA_CANARY)
        error = error | 1;
    if (((int *)stack->data)[stack->capacity*sizeof (Elem_t)/sizeof (int)] != RIGHT_DATA_CANARY)
        error = error | 2;
    return error;

}