#include "Runner.h"
#include <iostream>
using namespace std;

int main(int argc, char* argv[])
{ // runner进程程序，作为待测程度的容器
    
    // 先进行初始化，输入文件指向stdin，将stdout指向输出文件
    if (freopen(argv[2], "r", stdin) == NULL || freopen(argv[3], "w", stdout) == NULL)  {
        // 重定向过程出错。raise一个SIGUSR1（后期由主进程接受处理），终止运行。
	    raise(SIGUSR1);
        exit(EXIT_FAILURE);
    }
    string g(argv[1]);
    
    // 开始之前待测程序。
    execl(argv[1], ("./"+g).c_str(), NULL);
    
    // 无法启动待测程序，raise一个SIGUSR1（后期由主进程接受处理），终止运行。
    raise(SIGUSR1);
    exit(EXIT_FAILURE);
    
    return 0;
}
