/**
 * The Sepcial Judge Code Demo for KalznOJ
 * Return result to judge.
 * 
 * argv[1] is the input data file path.
 * argv[2] is the program output data file path.
 * argv[3] is the std output data file path.
 * 
 * 
 * If Sepcial Judge Program crash or can not exit in some seconds(default is 10s)
 * system will regard that is Wrong Answer.
 * 
 * Good Luck.
*/


#include <iostream>

#include <stdlib.h>
#define ACCEPTED 0
#define WRONG_ANSWER 1
#define TIME_LIMIT_EXCEEDED 2
#define MEMORY_LIMIT_EXCEEDED 3
#define RUNTIME_ERROR 4
#define PRESENTATION_ERROR 5
#define OUTPUT_LIMIT_EXCEEDED 6
#define SYSTEM_ERROR 7
int SPJ_RESULT = 0;


int main(int argc, char * argv[]) {
    
    FILE * input = fopen(argv[1], "r");
    FILE * output = fopen(argv[2], "r");
    FILE * stdoutput = fopen(argv[3], "r");

// Begin to write code.... 
    
    SPJ_RESULT =  ACCEPTED;

// End to write code......


    fclose(input);
    fclose(output);
    fclose(stdoutput);

    return SPJ_RESULT;

}
