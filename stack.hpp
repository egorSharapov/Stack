#ifndef STACK_HPP_INCLUDED
#define STACK_HPP_INCLUDED

#include "config.h"
#include "stdio.h"

const int int_poison  = 47789;


struct info
{
    const char *func_name = NULL;
    const char *file_name = NULL;
    const char *stack_name = NULL;
    int line = 0;
};

struct Stack_t
{
    int left_canary = 0;
    Elem_t *data = NULL;
    int hash = 0;
    int data_hash = 0;
    size_t size = 0;
    size_t capacity = 0;
    info stack_info = {};
    int right_canary = 0;
};

#endif