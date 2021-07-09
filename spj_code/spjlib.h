#ifndef SPJLIB_H
#define SPJLIB_H
#include <stdio.h>
#include <stdlib.h>
#define ACCEPTED 0
#define WRONG_ANSWER 1
#define START_JUDGE() FILE * input = fopen(argv[1], "r"); FILE * stdoutput = fopen(argv[2], "r"); FILE * output = fopen(argv[3], "r");
#define EXIT_JUDGE(result) fclose(input); fclose(output); fclose(stdoutput); return result;
#endif