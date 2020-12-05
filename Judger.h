/*
KalznAsawind judger
time: 20-12-4
By: c++
*/
#include <stdio.h>
#include <stdarg.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <grp.h>
#include <dlfcn.h>
#include <errno.h>
#include <sched.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mount.h>
#include <sys/wait.h>

#include <string>
#include <iostream>
using namespace std;

class JudgerResult{
public:
    const static int ACCEPTED = 0; 
    const static int WRONG_ANSWER = 1;
    const static int TIME_LIMIT_EXCEEDED = 2;
    const static int MEMORY_LIMIT_EXCEEDED = 3;
    const static int RUNTIME_ERROR = 4;
    const static int PRESENTATION_ERROR = 5; 
    const static int OUTPUT_LIMIT_EXCEEDED = 6; 
    const static int SYSTEM_ERROR = 7;
};

class JudgeConfig {

public:
    int MaxCPUTime;
    long long MaxMemory;
    long long MaxStack;
    long long MaxOutputSize;
    short CompilerCode;


    string ExePath;
    string InputDataPath;
    string OutputDataPath;
    string StdAnswerPath;
    string LogPath;
    bool isSPJ;
    string SPJExePath;

    JudgeConfig();
    
};



class RunConfig {
public:
    int UseCPUTime;
    long long UseMemory;
    long long UseMaxStack;
    long long UseOutputSize;
    int ExitSignal;
    int ExitCode;
    int Error;


    int Result;
    string ResultDetail;
};

class Judger {
private:
    static JudgeConfig config;
    const static int MAXBUF = 1e6;
    const static int MAX_OVERFLOW_MULTIPLE = 2;
    const static bool StrictMode = true;
    static string ansbuf, outbuf;
    static RunConfig CreateSystemError(string me);
    static void ExecKiller(string KillerExePath, pid_t RunnerId, int KillTime);
    static void ExecRunner(string RunnerExePath, string ExePath, string InputDataPath, string OutputDataPath);
    static void GetResult(RunConfig* result, int status, rusage resource_usage);
    static void CheckAnswer( RunConfig* result);
    static void FormatString (string &s);
public:
    static void run(JudgeConfig task, RunConfig* result, const string & RunnerExePath, const string & KillerExePath);
};
