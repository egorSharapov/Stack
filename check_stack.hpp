#ifndef CHECK_STACK_HPP_INCLUDED
#define CHECK_STACK_HPP_INCLUDED

#include <stdlib.h>
#include "stack.hpp"
#include "stack_functions.hpp"




#define stack_dump(output_file, Stack, mode) stack_dump_(output_file, Stack, mode, __FUNCTION__, __FILE__, __LINE__)

#define assert_ok(stack)  unsigned long errors = stack_ok (stack);    \
                          if (errors != 0)                            \
                          {                                           \
                              print_stack_error (errors);             \
                              stack_dump (output_file, stack, FULL);  \
                              exit(0);                                \
                          }


enum CANARIES
{
    LEFT_DATA_CANARY  = 47789,
    RIGHT_DATA_CANARY = 47789,
    LEFT_CANARY       = 47789,
    RIGHT_CANARY      = 47789,
};


enum ERRORS
{
    CAPACITY_SIZE_ERROR = 0,
    DATA_POINTER_ERROR  = 1,
    ZERO_CAPACITY_ERROR = 2,
    LEFT_CANARY_ERROR  = 3,
    RIGHT_CANARY_ERROR  = 4,
    LEFT_DATA_CANARY_ERROR = 5,
    RIGHT_DATA_CANARY_ERROR = 6,
    HASH_ERROR = 7,
};


enum DUMP_MODE
{
    FULL  = 0,
    SHORT = 1,
    NAME  = 2,
};


void   print_stack_data      (FILE *file, int index, int stack_data);
void   print_stack_data      (FILE *file, int index, double stack_data);
void   stack_dump_           (FILE *file, Stack_t *stack, DUMP_MODE mode, const char *func_name, const char *file_name, const int line);
void print_stack_error (unsigned long errors_code);
int    calculate_hash        (void * obj, size_t size);
void   calculate_stack_hash  (Stack_t * stack);
bool   check_stack_hash      (Stack_t *stack);
int    update_stack_hash     (Stack_t *stack);
Elem_t *create_data_canary   (Stack_t * stack);
Elem_t *recreate_data_canary (Stack_t * stack);

unsigned short data_canary_ok (Stack_t *stack);
unsigned short canary_ok      (Stack_t *stack);
unsigned long  stack_ok       (Stack_t *stack);

extern FILE *output_file;

#endif