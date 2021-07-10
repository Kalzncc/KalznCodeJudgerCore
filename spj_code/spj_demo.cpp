#include "spjlib.h"
int main(int argc, char * argv[]) {
    START_JUDGE();


//------------SPJ--CODE--------------------
    int a; fscanf(input, "%d", &a);
    int b; fscanf(output, "%d", &b);
    int c; fscanf(stdoutput, "%d", &c);

    if (/*some conditions*/ )
        EXIT_JUDGE(ACCEPTED); 
    else EXIT_JUDGE(WRONG_ANSWER); 
//------------SPJ--CODE--------------------


    EXIT_JUDGE(ACCEPTED); 
}
