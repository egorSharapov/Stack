#ifndef STACK_HPP_INCLUDED
#define STACK_HPP_INCLUDED

#include "config.h"
#include "stdio.h"

const int int_poison  = 47789;


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
    int hash;
    size_t size;
    size_t capacity;
    info stack_info;
    int right_canary;
};

#endif