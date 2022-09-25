#ifndef STACK_FUNCTIONS_HPP_INCLUDED
#define STACK_FUNCTIONS_HPP_INCLUDED
#include "config.h"
#include <stdio.h>
#include <stdlib.h>

#define int_poison 47789 
#define BEGIN_CANARY 47789
#define canary2 47789
#define R_DATA_CANARY 47789
#define stack_ctor(Stack , size_t) stack_ctor_(&Stack, size_t, #Stack, __FUNCTION__, __FILE__, __LINE__)
#define stack_dump(Stack, mode) stack_dump_(Stack, mode, __FUNCTION__, __FILE__, __LINE__)
//#define stack_dump(Stack) stack_dump_(Stack, FULL, __FUNCTION__, __FILE__, __LINE__)
#define assert_ok(stack)  if (stack_ok (stack) != 0)                 \
                          {                                          \
                              print_stack_error (stack_ok(stack));   \
                              stack_dump (stack, FULL);              \
                          }

struct info
{
    const char *func_name;
    const char *file_name;
    const char *stack_name;
    int line;
};

struct Stack_t
{
    int left_canary;
    Elem_t *data;
    size_t size;
    size_t capacity;
    info stack_info;
    int right_canary;
};


enum ERRORS
{
    CAPACITY_SIZE_ERROR = 0,
    DATA_POINTER_ERROR  = 1,
    ZERO_CAPACITY_ERROR = 2,
    CANARY_ERROR  = 3,
};

enum DUMP_MODE
{
    FULL  = 0,
    SHORT = 1,
    NAME  = 2,
};

void   *recalloc        (void *ptr, size_t num, size_t size);
void   stack_realloc    (Stack_t *stk);
void   stack_push       (Stack_t *stk, Elem_t value);
Elem_t stack_pop        (Stack_t *stk, int *err = NULL);
void   stack_ctor_      (Stack_t *stack, size_t capacity, const char * stack_name, const char * func_name, const char * file_name, const int line);
void   stack_dtor       (Stack_t *stack);
void   stack_fit        (Stack_t *stk);
void   print_stack_data (int index, int stack_data);
void   print_stack_data (int index, double stack_data);
void   stack_data_fill  (int *stack_data);
void   stack_data_fill  (double *stack_data);
void   stack_dump_      (Stack_t *stack, DUMP_MODE mode, const char *func_name, const char *file_name, const int line);
unsigned short  canary_ok        (Stack_t *stack);
unsigned long stack_ok(Stack_t *stack);
void   print_stack_error (unsigned long errors_code);

#endif
