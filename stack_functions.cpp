#include "config.h"
#include <string.h>
#include <math.h>
#include "log.h"
#include <malloc.h>
#include <assert.h>

static Elem_t * create_data_canary (Stack_t * stack)
{
    assert (stack);

    int * stack_data = (int *) calloc (1, (sizeof (LEFT_DATA_CANARY) + stack->capacity*sizeof(Elem_t) + sizeof (RIGHT_DATA_CANARY)));

    stack_data[0] = LEFT_DATA_CANARY;
    stack_data++;
    stack_data[stack->capacity*sizeof(Elem_t)/sizeof(int)] = RIGHT_DATA_CANARY;
    
    return (Elem_t * ) stack_data;
}

void stack_ctor_ (Stack_t *stack, size_t capacity, const char * stack_name, const char * func_name, const char * file_name, const int line)
{
    assert (stack);

    stack->capacity = capacity;
    stack->size = 0;

    stack->data = create_data_canary (stack);

    stack->left_canary = LEFT_CANARY;
    stack->right_canary = RIGHT_CANARY;
    
    stack->stack_info.func_name = func_name;
    stack->stack_info.file_name = file_name;
    stack->stack_info.stack_name = stack_name;
    stack->stack_info.line = line;
    
    
    for (int index = 0; index < capacity; index++)
    {
        stack_data_fill (&stack->data[index]);;
    }

    calculate_stack_hash (stack);

    assert_ok (stack);
}


void stack_data_fill (int *stack_data)
{
    assert (stack_data);

    *stack_data = int_poison;
}


void stack_data_fill (double *stack_data)
{
    assert (stack_data);

    *stack_data = NAN;
}

void stack_dtor (Stack_t *stack)
{
    assert (stack);

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

    if ((int)(num - temp_size) > 0)
        memset ((char *) ptr + temp_size, 0, num*size - temp_size);

    return ptr;
}


static Elem_t * recreate_data_canary (Stack_t * stack)
{
    assert (stack);
    
    int * stack_data = (int *) stack->data;
    
    stack_data--;

    stack_data = (int*) recalloc (stack_data, sizeof (LEFT_DATA_CANARY) + stack->capacity*sizeof (Elem_t) + sizeof (RIGHT_DATA_CANARY), 1);

    stack_data++;

    stack_data[stack->capacity*sizeof (Elem_t)/sizeof(int)] = RIGHT_DATA_CANARY;
    
    return (Elem_t *) stack_data;
}

void stack_realloc (Stack_t *stack)
{
    assert (stack);

    stack->capacity *= multiple;
    
    stack->data = recreate_data_canary (stack);

    for (int index = stack->size; index < stack->capacity; index++)
        stack_data_fill(&stack->data[index]);
}


void stack_push (Stack_t *stack, Elem_t value)
{
    assert (stack);
    assert_ok (stack);

    if (stack->size >= stack->capacity)
        stack_realloc (stack);

    stack->data[stack->size++] = value;

    calculate_stack_hash (stack);
}


Elem_t stack_pop (Stack_t *stack, int *err)
{
    assert (stack);
    assert_ok (stack);

    Elem_t value = 0;

    if (err != NULL ) *err = 1;

    if (stack->size > 0)
    {
        value = stack->data[--stack->size];
        stack_data_fill (&stack->data[stack->size]);
        stack_fit (stack);
        if (err != NULL ) *err = 0;
        calculate_stack_hash (stack);
    }
   

    return value;
}

void stack_dump_ (Stack_t *stack, DUMP_MODE mode, const char *func_name, const char *file_name, const int line)
{
    assert (stack);
      
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
    assert (stack);

    if (stack->size*multiple <= stack->capacity and stack->size != 0)
    {
        stack->capacity /= multiple;
        stack->data = recreate_data_canary (stack);

    }
}

unsigned long stack_ok(Stack_t *stack)
{
    assert (stack);
    
    unsigned long error = 0;

    if  ((int)stack->size > (int)stack->capacity)
        error = error | (1 << CAPACITY_SIZE_ERROR);
    
    if (stack->data == NULL)
        error = error | (1 << DATA_POINTER_ERROR);
    else
        error = error | (data_canary_ok (stack) << LEFT_DATA_CANARY_ERROR);

    if (((int) stack->capacity < 0))
        error = error | (1 << ZERO_CAPACITY_ERROR);
    else
    
    if (!error )error = error | (check_stack_hash (stack) << HASH_ERROR);

    error = error | (canary_ok (stack) << LEFT_CANARY_ERROR);

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
        hash += point[size];
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
        printf ("WRONG CAPACITY\n");
    
    if (errors_code >> DATA_POINTER_ERROR & 1)
        printf ("DATA POINTER TO NULL\n");
    
    if (errors_code >> ZERO_CAPACITY_ERROR & 1)
        printf ("ZERO CAPACITY\n");
    
    if (errors_code >> LEFT_CANARY_ERROR & 1)
        printf ("LEFT CANARY ERROR\n");
    if (errors_code >> RIGHT_CANARY_ERROR & 1)
        printf ("RIGHT CANARY ERROR\n");
    
    if (errors_code >> LEFT_DATA_CANARY_ERROR & 1)
        printf ("LEFT DATA CANARY ERROR\n");
    
    if (errors_code >> RIGHT_DATA_CANARY_ERROR & 1)
        printf ("RIGHT DATA CANARY ERROR\n");

    if (errors_code >> HASH_ERROR & 1)
        printf ("HASH ERROR\n");
}

unsigned short canary_ok (Stack_t *stack)
{
    unsigned short error = 0;
    if (stack->left_canary != LEFT_CANARY)
        error += 1;
    else if (stack->right_canary != RIGHT_CANARY)
        error += 2;

    return error;

}

unsigned short data_canary_ok (Stack_t *stack)
{
    assert (stack);

    unsigned short error = 0;
    if (((int *)stack->data)[-1] != LEFT_DATA_CANARY)
        error += 1;
    if (((int *)stack->data)[stack->capacity*sizeof (Elem_t)/sizeof (int)] != RIGHT_DATA_CANARY)
        error += 2;
    return error;

}