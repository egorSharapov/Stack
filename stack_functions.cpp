#include "config.h"
#include <string.h>
#include <math.h>
#include "log.h"
#include <malloc.h>


void stack_ctor_ (Stack_t *stack, size_t capacity, const char * stack_name, const char * func_name, const char * file_name, const int line)
{
    stack->capacity = capacity;
    stack->size = 0;
    stack->data = (Elem_t *) calloc (sizeof (Elem_t), (capacity));
    stack->left_canary = BEGIN_CANARY;
    stack->right_canary = canary2;
    stack->stack_info.func_name = func_name;
    stack->stack_info.file_name = file_name;
    stack->stack_info.stack_name = stack_name;
    stack->stack_info.line = line;
    
    
    for (int index = 0; index < capacity; index++)
    {
        stack_data_fill (&stack->data[index]);;
    }
}


void stack_data_fill (int *stack_data)
{
    *stack_data = int_poison;
}


void stack_data_fill (double *stack_data)
{
    *stack_data = NAN;
}

void stack_dtor (Stack_t *stack)
{
    stack->capacity = -1;
    stack->size = -2;
    stack->stack_info = {};
    free (stack->data);
    stack = NULL;
}

void *recalloc(void *ptr, size_t num, size_t size)
{
    size_t temp_size = _msize(ptr);
    
    ptr = realloc (ptr, size*num);

    memset ((char *) ptr + temp_size, 0, num*size - temp_size);

    return ptr;
}

void stack_realloc (Stack_t *stack)
{
    stack->capacity *= multiple;

    stack->data = (Elem_t*) recalloc (stack->data, stack->capacity, sizeof(Elem_t));
    
    for (int index = stack->size; index < stack->capacity; index++)
    {
        stack_data_fill(&stack->data[index]);
    }
}


void stack_push (Stack_t *stack, Elem_t value)
{
    assert_ok (stack);

    if (stack->size >= stack->capacity)
        stack_realloc (stack);

    stack->data[stack->size++] = value;
}


Elem_t stack_pop (Stack_t *stack, int *err)
{
    assert_ok (stack);
    
    Elem_t value = 0;

    if (stack->size > 0)
    {
        value = stack->data[--stack->size];
        stack_data_fill (&stack->data[stack->size]);
        stack_fit (stack);
        err = 0;
    }
    else
        *err = 1;

    return value;
}

void stack_dump_ (Stack_t *stack, DUMP_MODE mode, const char *func_name, const char *file_name, const int line)
{
    const char * error = "ok";
    
    if (stack_ok (stack) != 0)
        error  = "ERROR";

    printf ("%s() at %s.cpp (%d)\n", func_name, file_name, line);
    printf ("stack[%p] (%s) \"%s\" ", stack, error, stack->stack_info.stack_name);
    printf ("at %s() at %s (%d)\n", stack->stack_info.func_name, stack->stack_info.file_name, stack->stack_info.line);
    printf ("{\n size     = %d\n capacity = %d\n\n", stack->size, stack->capacity);

    if (mode == FULL or stack->capacity <= 6)
        for (int index = 0; index < stack->capacity; index++)
            print_stack_data (index, stack->data[index]);
    
    else if (mode == SHORT)
    {
        for (int index = 0; index < 3; index++) 
            print_stack_data (index, stack->data[index]);
        
        printf ("\n........\n\n");
        
        for (int index = stack->capacity - 3; index < stack->capacity; index++) 
            print_stack_data (index, stack->data[index]);
    }
    
    printf ("}\n\n");
}


void print_stack_data (int index, int stack_data)
{
    if (stack_data != int_poison)
        printf ("*[%d] = %d\n", index, stack_data);
    else
        printf (" [%d] = %X (poison)\n", index, stack_data);
}


void print_stack_data (int index, double stack_data)
{
    if (!isnan(stack_data))
        printf ("*[%d] = %lg\n", index, stack_data);
    else
        printf (" [%d] = NAN (poison)\n", index);
}



void stack_fit (Stack_t *stack)
{
    if (stack->size*2 <= stack->capacity and stack->size != 0)
    {
        stack->capacity /= 2;
        stack->data = (Elem_t *) recalloc (stack->data, stack->capacity, sizeof(Elem_t));
    }
}

unsigned long stack_ok(Stack_t *stack)
{
    unsigned long error = 0;
    
    if  (stack->size > stack->capacity)
        error = error | (1 << CAPACITY_SIZE_ERROR);
    
    if (stack->data == NULL)
        error = error | (1 << DATA_POINTER_ERROR);
    
    if (((int) stack->capacity < 0))
        error = error | (1 << CAPACITY_SIZE_ERROR);
    
    error = error | (canary_ok (stack) << CANARY_ERROR);

    return error;
}


void calculate_hash ();
bool check_hash ();
void update_hash ();

void print_stack_error (unsigned long errors_code)
{

    if (errors_code >> CAPACITY_SIZE_ERROR & 1)
        printf ("WRONG CAPACITY\n");
    if (errors_code >> DATA_POINTER_ERROR & 1)
        printf ("DATA POINTER TO NULL\n");
    if (errors_code >> ZERO_CAPACITY_ERROR & 1)
        printf ("ZERO CAPACITY\n");
    if (errors_code >> CANARY_ERROR & 1)
        printf ("CANARY ERROR\n");
}

unsigned short canary_ok (Stack_t *stack)
{
    unsigned short error = 0;
    if (stack->left_canary != BEGIN_CANARY)
        error += 1;
    else if (stack->right_canary != BEGIN_CANARY)
        error += 2;

    return error;
}