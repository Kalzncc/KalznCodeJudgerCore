#ifndef SPJLIB_H
#define SPJLIB_H
#include <stdio.h>
#include <stdlib.h>


typedef int judge_result;

#define ACCEPTED        0
#define WRONG_ANSWER    1

#define START_JUDGE() \
        FILE *input = fopen(argv[1], "r"); \
        FILE *stdoutput = fopen(argv[2], "r"); \
        FILE *output = fopen(argv[3], "r");

#define EXIT_JUDGE(result) \
        fclose(input); \
        fclose(output); \
        fclose(stdoutput); \
        exit(result);

#define SPJ_FLOAT_CMP(x, y, dif) \
        ( ((x)-(y)<(dif)) && ((y)-(x)<(dif)) )




#endif