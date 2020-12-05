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
    const static int ACCEPTED = 0;  // 答案正确
    const static int WRONG_ANSWER = 1; // 答案错误
    const static int TIME_LIMIT_EXCEEDED = 2; //时间超限(CPU时间超限)
    const static int MEMORY_LIMIT_EXCEEDED = 3; //内存超限
    const static int RUNTIME_ERROR = 4; // 运行时错误
    const static int PRESENTATION_ERROR = 5;  // 格式错误(只在严格评测模式)
    const static int OUTPUT_LIMIT_EXCEEDED = 6;  //输出超限
    const static int SYSTEM_ERROR = 7; //系统错误(错误信息查看RunConfig中的ResultDetail)
};

class JudgeConfig {

public:
    int MaxCPUTime; //最大CPU时限
    long long MaxMemory;  //最大内存使用量
    long long MaxStack; //最大栈
    long long MaxOutputSize; //最大输出大小
    short CompilerCode; // 编译器代码


    string ExePath; // 带评测程序路径
    string InputDataPath; //输入文件路径
    string OutputDataPath; //输出文件路径
    string StdAnswerPath; //标准答案路径
    string LogPath; //log路径
    bool isSPJ; //是否使用SPJ
    string SPJExePath;//SPJ路径

    JudgeConfig();
    
};



class RunConfig {
public:
    int UseCPUTime; //CPU时间消耗
    long long UseMemory;//内存消耗
    long long UseMaxStack;//栈消耗
    long long UseOutputSize;//输出大小
    int ExitSignal;//结束信号量
    int ExitCode;//退出代码
    int Error;//错误代码


    int Result;//结果
    string ResultDetail;//结果解释
};

class Judger {
private:
    static JudgeConfig config;
    const static int MAXBUF = 1e6; //数据字符最大缓存
    const static int MAX_OVERFLOW_MULTIPLE = 2; //输出最大倍数
    /*
    在评测时，首先读入标准答案，然后读入待评测程序的输出，如果待评测程序的输出大小大于标准答案的MAX_OVERFLOW_MULTIPLE倍，
    即返回结果OUTPUT_LIMIT_EXCEEDED 输出超限
    */


    const static bool StrictMode = true;
    /*
    严格评测模式：
    如果为true，则启用严格评测模式，在严格模式下，按照以下顺序比对答案和输出：
    1、如果输出或者答案最后有一个'\n'，则删掉一个'\n'。
    2、如果输出和答案相等，则AC。
    3、否则：将答案和输出的所有连续空白字符都用一个空格替代。
    4、如果此时输出和答案相等，则PE。
    5、否则WA。

    在非严格模式下：
    1、如果输出或者答案最后有一个'\n'，则删掉一个'\n'。
    3、将答案和输出的所有连续空白字符都用一个空格替代。
    4、如果此时输出和答案相等，则AC。
    5、否则WA。

    */
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
