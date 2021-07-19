#include "spjlib.h"
#include <cmath>
int main(int argc, char * argv[]) {
    START_JUDGE();


//------------SPJ--CODE--------------------
    double a; 
    fscanf(output, "%lf", &a);
    double b;
    fscanf(stdoutput, "%lf", &b);
    if (fabs(a - b) < 1e-4) {
        EXIT_JUDGE(ACCEPTED);
    }
    EXIT_JUDGE(WRONG_ANSWER);
//------------SPJ--CODE--------------------


    EXIT_JUDGE(ACCEPTED); 
}
