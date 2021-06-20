#include "Judger.h"
#include <iostream>

// % everyone
#include <cstdio>
#include<iostream>
#include<cstring>
#include <map>
#include <queue>
#include <set>
#include <cstdlib>
#include <cmath>
#include <algorithm>
#include <vector>
#include <string>
#include <list>
#include <cctype>
#include <time.h>

namespace IO {
    double start_time = 0.0;
    void ct() { start_time = clock(); return; }
    void fast_cin() { std::ios::sync_with_stdio(false); std::cin.tie(); }
    void read_f(int flag = 0) { freopen("0.in", "r", stdin); if(!flag) freopen("0.out", "w", stdout); }
    void run_time() { std::cout << "\nESC in : " << ( clock() - start_time ) * 1000.0 / CLOCKS_PER_SEC << "ms" << std::endl; }
}
using namespace IO;
template <typename T>
bool bacmp(const T & a, const T & b) { return a > b; }
template <typename T>
bool pecmp(const T & a, const T & b) { return a < b; }

#define ll long long
#define ull unsigned ll
#define _min(x, y) ((x)>(y)?(y):(x))
#define _max(x, y) ((x)>(y)?(x):(y))
#define max3(x, y, z) ( max( (x), max( (y), (z) ) ) )
#define min3(x, y, z) ( min( (x), min( (y), (z) ) ) )
#define pr make_pair
#define pb push_back
using namespace std;

using namespace std;
JudgeConfig::JudgeConfig() {
    MaxCPUTime = 30;
    MaxMemory = MaxOutputSize = MaxStack = 0;
    CompilerCode = 0;
    ExePath = "run";
    InputDataPath = "input.in";
    OutputDataPath = "output.out";
    StdAnswerPath = "ans.out";
    LogPath = "Judge.log";
    isSPJ = false;
    
    SPJExePath = "";
}
string Judger::ansbuf;
string Judger::outbuf;
JudgeConfig Judger::config;
RunConfig Judger::CreateSystemError(string me) {
    RunConfig result;
    result.Result = JudgerResult::SYSTEM_ERROR;
    result.ResultDetail = me;
    
    /* log file!
    FILE * fp = fopen(config.LogPath.c_str(), "a+");
    if (fp == NULL) {
        result.ResultDetail += " and Fail to write log file";
    } else {
        fprintf(fp, "SYSTEM_ERROR : %s\n", result.ResultDetail);
    }
    */

    return result;
}
void Judger::ExecKiller(string KillerExePath, pid_t RunnerId, int KillTime) {
    execl(KillerExePath.c_str(), ("./"+KillerExePath).c_str(), to_string(RunnerId).c_str(), to_string(KillTime).c_str(), NULL);
}
void Judger::ExecRunner(string RunnerExePath, string ExePath, string InputDataPath, string OutputDataPath) {
    execl(RunnerExePath.c_str(), ("./"+RunnerExePath).c_str(), ExePath.c_str(), InputDataPath.c_str(), OutputDataPath.c_str(), NULL);
}
void Judger::ExecSPJ(string SPJExePath, string InputDataPath, string AnswerDataPath, string OutputDataPath) {
    execl(SPJExePath.c_str(), ("./"+SPJExePath).c_str(), InputDataPath.c_str(), AnswerDataPath.c_str(), OutputDataPath.c_str(), NULL);
}
void Judger::GetResult(RunConfig* result, int status, rusage resource_usage) {
    
    // 首先默认AC
    result->Result = JudgerResult::ACCEPTED;
    
    // 截取结束信号量
    result->ExitSignal=WTERMSIG(status);

    // 自定义的信号量，出现此错误，表明runner在初始化时出错了，例如无法打开输入数据。
    // 不是待测程序的错误，属于系统错误。
    if(result->ExitSignal == SIGUSR1) {
        *result = CreateSystemError("Complex error in runner (check log in details).");
        return;
    }

    // 截取返回值
    // 如果返回值不是0，则判为非0返回，属于RE。（由于此部分由交题者控制，也可省略）
    result->ExitCode = WEXITSTATUS(status);
    if (result->ExitCode != 0) {
        result->Result = JudgerResult::RUNTIME_ERROR;
        result->ResultDetail="Non zero return";
    }
	
    // 获取运行资源数据
    result->UseCPUTime = (int) (resource_usage.ru_utime.tv_sec * 1000 + resource_usage.ru_utime.tv_usec / 1000);
    result->UseMemory = resource_usage.ru_maxrss * 1024;
	
    
    if (result->ExitSignal == SIGSEGV) { // 信号量为SIGSEGV 证明程序为正常终止。
        
        if (result->UseMemory > config.MaxMemory ) { // 由于内存需求过大，无法分配导致的崩溃，如果超过题目要求，属于内存超限
            result->Result = JudgerResult::MEMORY_LIMIT_EXCEEDED;
            result->ResultDetail="Memory limit exceeded : Leading to Judger stop. t";
        } else {  // 其他情况就是因为访存异常、爆栈、或者除0导致的。
            result->Result = JudgerResult::RUNTIME_ERROR;
            result->ResultDetail="runtime error exceeded : Leading to Judger stop.";
        }
    } else {
        if (result->ExitSignal != 0) {  // 只有0才是正常的。
            result->Result = JudgerResult::RUNTIME_ERROR;
            result->ResultDetail="runtime error exceeded : Leading to Judger stop. p";
        }
        if ( result->UseMemory > config.MaxMemory) { // 内存超限
            result->Result = JudgerResult::MEMORY_LIMIT_EXCEEDED;
            result->ResultDetail="Memory limit exceeded.";
        }
        if (result->UseCPUTime > config.MaxCPUTime) { // 时间超限
            result->Result = JudgerResult::TIME_LIMIT_EXCEEDED;
            result->ResultDetail="Time limit exceeded.";
        }
    }
}
void Judger::GetResultSPJ(RunConfig* result, int status, rusage resource_usage) {
    if(WTERMSIG(status) != 0) { // SPJ没有正常退出，视为WA。
        result->Result = JudgerResult::WRONG_ANSWER;
        result->ResultDetail = "SPJ.";
        return;
    }
    result->Result = WEXITSTATUS(status); // 参见SPJ程序实例的说明
    result->ResultDetail = "SPJ.";
}
void Judger::FormatString (string & s) {
    // 格式化字符串，格式化方式：
    // 1、将开头和结尾的空白字符全部删掉
    // 2、将之间的所有连续空白字符都由一个空格替代。
    int cnt = 0;
    int pos = 0;
    while(pos < s.size() && isspace(s[pos])) pos++;
    while(pos < s.size()) {
        s[cnt++] = s[pos++];
        if (isspace(s[pos])) {
            s[cnt++] = ' ';
            while(pos < s.size() && isspace(s[pos])) pos++;
        }
    }
    while(s.size() > cnt || (s.size()!=0 && isspace(*s.rbegin() ) ) ) s.pop_back();

    return;
}
void Judger::StrictFormatWindowsString(string & s) {
    // 在严格模式格式化字符，解决Windows于liunx换行不匹配的问题。
    // 由于在windows下，记事本默认换行是"\r\n"。可能有出题人手造样例，对于所有"\r\n"都删减前面的 \r
    int cnt = 0;
    for (int i = 0; i <s.length()-1; i++) {
        if (s[i] == '\r' && s[i+1] == '\n') {
            s[cnt++] = '\n';
        }
        else s[cnt++] = s[i];
    }
    while(s.size() > cnt) s.pop_back();
    return;
}
void Judger::CheckAnswer( RunConfig* result) {
    
    // 匹配答案和输出
    FILE * out = fopen(config.OutputDataPath.c_str(), "r");
    FILE * ans = fopen(config.StdAnswerPath.c_str(), "r");
    if (out == NULL || ans == NULL) {
        *result = CreateSystemError("Fail to open data file.");
        return;
    }
    char ch;

    //将答案和输出先读入到缓冲区
	ansbuf.clear(); outbuf.clear();


    while( (ch = fgetc(ans) )!=EOF )  { // 答案大小大于系统预设的最大值，这里是一个安全处理。
                                        // 管理者根据机器性能预设最大值。防止撑爆评测机内存导致崩溃
                                        // 系统问题
        ansbuf.push_back(ch);
        if (ansbuf.size() > MAXBUF) {
            *result = CreateSystemError("Answer Data too large.");
            return;
        }
    }
    while( (ch = fgetc(out) )!=EOF )  { // 预设，如果待测程序输出过大，大于了预设的倍数，就输出OLE。
        outbuf.push_back(ch);
        if (!AbsolteOutputLimitMode) {
            if (outbuf.size() > ansbuf.size() * MAX_OVERFLOW_MULTIPLE) {
                result->Result = JudgerResult::OUTPUT_LIMIT_EXCEEDED;
                result->ResultDetail = "output too large.";
                return;
            }
        } else {
            if (outbuf.size() > config.MaxOutputSize) {
                result->Result = JudgerResult::OUTPUT_LIMIT_EXCEEDED;
                result->ResultDetail = "output too large.";
                return;
            }
        }
    }
    
    if (StrictMode) { // 严格评测模式 进行格式化后，只删去最后一个回车。
        StrictFormatWindowsString(ansbuf); StrictFormatWindowsString(outbuf);
        if (*(outbuf.rbegin()) == '\n') outbuf.pop_back();
        if (*(ansbuf.rbegin()) == '\n') ansbuf.pop_back();
        if (outbuf == ansbuf) { // 一字不差算作AC
            result->Result = JudgerResult::ACCEPTED;
            result->ResultDetail = "Ok.";
        } else { // 没有一字不差，确认是否为WA或者PE。
            FormatString(ansbuf); FormatString(outbuf);
            if ( ansbuf!=outbuf ) {
                result->Result = JudgerResult::WRONG_ANSWER;
                result->ResultDetail = "Output is different from std answer after ignore space characters.";
            } else {
                result->Result = JudgerResult::PRESENTATION_ERROR;
                result->ResultDetail = "Output is accepted! but presentation error.";
            }
        }
    } else { // 非严格模式， 之间格式化后再比较，如果一样就AC。
        FormatString(ansbuf); FormatString(outbuf);
        if ( ansbuf!=outbuf ) {
            result->Result = JudgerResult::WRONG_ANSWER;
            result->ResultDetail = "Output is different from std answer after ignore space characters.";
        } else {
            result->Result = JudgerResult::ACCEPTED;
            result->ResultDetail = "Ok.";
        }
    }
}

void Judger::run(JudgeConfig task, RunConfig* result, const string & RunnerExePath, const string & KillerExePath) {
	
    config = task;

    pid_t killer_id, runner_id;
    runner_id = fork();
    // Fork一个进程，作为运行runner的容器。

    if (runner_id<0) {
        *result = CreateSystemError("Fail to fork runner process.");
        return;
    }

    if (runner_id == 0) {
        // 若为fork出来的子进程， 将输入输出数据传入runner容器。开始执行runner进程。
        ExecRunner(RunnerExePath, task.ExePath, task.InputDataPath, task.OutputDataPath);
        *result = CreateSystemError("Fail to execute runner process.");
        exit(EXIT_FAILURE);
    } else {

        killer_id = fork();
        // Fork一个进程，作为运行killer的容器
        if (killer_id < 0) {
            *result = CreateSystemError("Fail to fork killer process.");
            return;
        }
        
        if (killer_id == 0) {
            // 若为fork出来的子进程， 将runner的进程id和最大耗时传入killer进程，开始执行。
            ExecKiller(KillerExePath, runner_id, task.MaxCPUTime);
            *result = CreateSystemError("Fail to execute killer process.");
            exit(EXIT_FAILURE);
        } else {

            // 此为主进程
            int killer_status, runner_status;
            struct rusage resource_usage, resource_usage2;

            if (wait4(runner_id, &runner_status, WSTOPPED, &resource_usage) == -1) {
                //等待runner停止运行，如果返回-1，则出错。 然后kill掉killer进程，报错退出。
                 kill(killer_id, SIGKILL);
                *result = CreateSystemError("Fail to wait runner process.");
                return;
            }
            
            // runner已经结束，待测程序已退出。kill掉killer进程。
            kill(killer_id, SIGKILL);
            
            
            if (wait4(killer_id, &killer_status, WSTOPPED, &resource_usage2) == -1) {
                // 等待killer退出（之前已经退出了，所以就是收集killer进程的数据） 如果返回-1，则出错, 报错退出。
                *result = CreateSystemError("Fail to wait killer process.");
                return;
            }
            
            // 通过runner的退出状态和资源使用情况评判程序是否超时、超内存、或者非正常退出。
            GetResult(result, runner_status, resource_usage);

            // 如果结果不为AC，则为WA之外的问题，直接退出。
            if (result->Result != JudgerResult::ACCEPTED) {
                return;
            }

            // 开始评判是否为WA（或者PE）。
            if (config.isSPJ == false) { // 如果不是SPJ，开始对比输出和答案文件
                CheckAnswer(result);
            } else {
                // 开始SPJ过程。

                pid_t spj_id = fork(); 
                // Fork一个进程，作为执行spj程序的容器
                if (spj_id < 0) {
                    *result = CreateSystemError("Fail to fork killer process.");
                    return;
                } 
                
                
                if (spj_id == 0) {

                    // 向spj传入输入数据、答案和输出数据。 开始执行spj。
                    ExecSPJ(task.SPJExePath, task.InputDataPath, task.StdAnswerPath, task.OutputDataPath);
                    *result = CreateSystemError("Fail to execute SPJ process.");
                    exit(EXIT_FAILURE);
                } else {
                    pid_t spjkiller_id = fork();
                    // 由于spj也由用户上传，无法保证正确性，这里仍使用一个killer监控其是否超时，默认时间为10s。

                    // 以下同理于runner。
                    if (spjkiller_id < 0) {
                        *result = CreateSystemError("Fail to fork killer process.");
                        return;
                    }
                    if (spjkiller_id == 0) {
                        ExecKiller(KillerExePath, spj_id, Judger::SPJ_MAX_TIME);
                        *result = CreateSystemError("Fail to execute killer process.");
                        exit(EXIT_FAILURE);
                    } else {
                        int spjkiller_status, spj_status;
                        struct rusage resource_usage, resource_usage2;
                        if (wait4(spj_id, &spj_status, WSTOPPED, &resource_usage) == -1) {
                            kill(spjkiller_id, SIGKILL);
                            *result = CreateSystemError("Fail to wait spj process.");
                            return;
                        }
                        kill(spjkiller_id, SIGKILL);
                        if (wait4(spjkiller_id, &spjkiller_status, WSTOPPED, &resource_usage2) == -1) {
                            *result = CreateSystemError("Fail to wait spj killer process.");
                            return;
                        }

                        // 传入spj状态 通过spj获取答案
			            GetResultSPJ(result, spj_status, resource_usage);
			
		            }
		
                }
            }
        }
    }

    return;
}

#include <iostream>


int main(int argc, char * argv[]) { //测试用主程序， 测试程序将从同目录下的0.in读入配置，然后将评测结果输出至同目录下的0.out
    read_f();

    JudgeConfig sst = JudgeConfig();
    sst.MaxOutputSize = 10000000;
    sst.MaxStack = 10000000;
    int te;
    cin >> te;
    sst.MaxCPUTime = te;
    cin >> te;
    sst.MaxMemory = te;
    sst.CompilerCode = 0;
    string tt; cin >> tt;
    sst.ExePath = tt;
    cin >> tt;
    sst.InputDataPath = tt;
    cin >> tt;
    sst.OutputDataPath = tt;
    cin >> tt;
    sst.StdAnswerPath = tt;
    sst.LogPath = "Judge.log";
    cin >> te;
    sst.isSPJ = te;
    cin >> tt;
    sst.SPJExePath = tt;

    string a, b; cin >> a >> b;

    RunConfig result = RunConfig();

    Judger::run(sst, &result, a, b);

    string res;
    switch (result.Result)
    {
    case 0:
        res = "AC";
        break;
    case 1 :
        res = "WA";
        break;
    case 2:
        res = "TLE";
        break;
    case 3:
        res = "MLE";
        break;
    case 4:
        res = "RE";
        break;
    case 5:
        res = "PE";
        break;
    case 6:
        res = "OLE";
        break;
    case 7:
        res = "SE";
    default:
        break;
    }
    cout << "Result : " << res << endl;
    cout << "Time(ms) : " << result.UseCPUTime << endl;
    cout << "Mem(kb) : " << result.UseMemory << endl;
    cout << "Stack : " << result.UseMaxStack << endl;
    cout << "ExitCode : " << result.ExitCode << endl;
    cout << "Detial : " << result.ResultDetail << endl;
    return 0;
}
