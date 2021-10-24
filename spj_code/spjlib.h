#ifndef SPJLIB_H
#define SPJLIB_H
#include <stdio.h>
#include <stdlib.h>

typedef int judge_result;

#define ACCEPTED                   0
#define WRONG_ANSWER               1

#define _CAN_NOT_OPEN_DATA_FILE    2
#define _ARGV_ERROR                3


void _start_judge(int argc, char *argv[], FILE* &in, FILE* &sout, FILE* &out);

#define START_JUDGE() \
        FILE *in = NULL; \
        FILE *sout = NULL; \
        FILE *out = NULL; \
        _start_judge(argc, argv, in, sout, out);
        

#define EXIT_JUDGE(result) \
        fclose(in); \
        fclose(out); \
        fclose(sout); \
        exit(result);




#endif