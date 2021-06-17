# Eng (chinglish) ver

This is just a toy made by me. It is very simple and implement the base function of code test.

#### what the program can do so far:
1) Test a executable file, monitor the cpu time and memory  resource consumeption.
2) Judge whether a program exit normally (Judge whether a program have a circumstance that leads to "runtime error" such as array access exception, stack exception, or divided by 0 and so on.)
3) Kill the timeout process.
4) Match the output and the standard answer, justify the result and format.
5) Compare the resource consumed and limited, test the program whether exceed the limitation of time or memory.
6) Two judge mode--strict and not strict is supplied.

#### what the program plan to do but is not implemented so far:
1) Monitor stack consumption.
2) Connect a SPJ program to test the file.
3) Kill memory limitation exceed process (have no idea to monitor the memory consumption during process is running).
4) The judgement of "Output limit exceeded" is not implemented.
5) Log information of judgement particular the system error.

Just debug in centOS7, suit all Liunx release version theoretically.

#### start try to use
Enter in the directory
```sh
g++ -std=c++11 Judger.h Judger.cpp -o Judger
g++ -std=c++11 Runner.h Runner.cpp -o runner
g++ -std=c++11 Killer.h Killer.cpp -o killer
```
Wait for updating.
