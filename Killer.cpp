#include "Killer.h"
#include <iostream>
using namespace std;
int Start_time = 1;
void start_time(int num) {
    Start_time = 0;
}
int main(int argc, char* argv[])
{
    cout << "Killer::" << endl;
    pid_t runner_id = atoi(argv[1]);
    int time = atoi(argv[2]);
    
   // kill(runner_id, SIGUSR2);
    sleep(time);
	cout << "ttt" << endl;
    kill(runner_id, SIGKILL);
    return 0;
}
