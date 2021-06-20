#include "Killer.h"
#include <iostream>
using namespace std;

int main(int argc, char* argv[])
{ // killer进程程序。
  // killer进程的任务是负责监控一个进程，在他超时的时候截断它。

    pid_t runner_id = atoi(argv[1]);
    int time = atoi(argv[2]);
    
    //传入时间以ms为单位，这里除1000，并预留2s的富足时间。
	time = time /1000 + 2;

    sleep(time);
    // 目标进程已经超时，killer进行截断
    kill(runner_id, SIGKILL);
    return 0;
}
