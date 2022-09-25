#ifndef LOG_HPP_INCLUDED
#define LOG_HPP_INCLUDED

#include <stdio.h>

#ifndef LOG_NAME
#define LOG_NAME "log.txt"
#endif

#ifndef NDEBUG
    #define $$$ (cmd) printf(#cmd);cmd
    #define $ fprintf(stdout, "\ni am here, in file: %s, in function: %s, in line: %d\n",__FILE__, __FUNCTION__, __LINE__);
#else
    #define $$$ (cmd) cmd
    #define $ ;
#endif

#endif // LOG_HPP_INCLUDED