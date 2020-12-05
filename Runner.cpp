#include "Runner.h"
#include <iostream>
using namespace std;
int Start_time = 1;
void start_time(int num) {
    Start_time = 0;
}
int main(int argc, char* argv[])
{
    cout << "Runner::" << endl;
   // signal(SIGUSR2, start_time);
    if (freopen(argv[2], "r", stdin) == NULL || freopen(argv[3], "w", stdout) == NULL)  {
	    raise(SIGUSR1);
        exit(EXIT_FAILURE);
    }
    string g(argv[1]);
    execl(argv[1], ("./"+g).c_str(), NULL);
    raise(SIGUSR1);
    exit(EXIT_FAILURE);
    return 0;
}
