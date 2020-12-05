#include "Judger.h"
#include <iostream>
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
	//cout << RunnerExePath << " " << ("./"+RunnerExePath).c_str() << " " << InputDataPath.c_str() << " " << OutputDataPath.c_str() << endl;
    execl(RunnerExePath.c_str(), ("./"+RunnerExePath).c_str(), ExePath.c_str(), InputDataPath.c_str(), OutputDataPath.c_str(), NULL);
}
void Judger::GetResult(RunConfig* result, int status, rusage resource_usage) {
    result->Result = JudgerResult::ACCEPTED;
    result->ExitSignal=WTERMSIG(status);
    cout << result->ExitSignal << " "<<  SIGUSR1 << endl;
    if(result->ExitSignal == SIGUSR1) {
        *result = CreateSystemError("Complex error in runner (check log in details).");
        return;
    }

    result->ExitCode = WEXITSTATUS(status);
    result->UseCPUTime = (int) (resource_usage.ru_utime.tv_sec * 1000 + resource_usage.ru_utime.tv_usec / 1000);
    result->UseMemory = resource_usage.ru_maxrss * 1024;
	
    if (result->ExitCode != 0) {
        result->Result = JudgerResult::RUNTIME_ERROR;
        result->ResultDetail="Non zero return";
    }
	
    if (result->ExitSignal == SIGSEGV) {
        if (result->UseMemory > config.MaxMemory ) {
            result->Result = JudgerResult::MEMORY_LIMIT_EXCEEDED;
            result->ResultDetail="Memory limit exceeded : Leading to Judger stop. t";
        } else {
            result->Result = JudgerResult::RUNTIME_ERROR;
            result->ResultDetail="runtime error exceeded : Leading to Judger stop.";
        }
    } else {
        if (result->ExitSignal != 0) {
            result->Result = JudgerResult::RUNTIME_ERROR;
            result->ResultDetail="runtime error exceeded : Leading to Judger stop. p";
        }
        if ( result->UseMemory > config.MaxMemory) {
            result->Result = JudgerResult::MEMORY_LIMIT_EXCEEDED;
            result->ResultDetail="Memory limit exceeded.";
        }
        if (result->UseCPUTime > config.MaxCPUTime) {
            result->Result = JudgerResult::TIME_LIMIT_EXCEEDED;
            result->ResultDetail="Time limit exceeded.";
        }
    }
}
void Judger::FormatString (string & s) {
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
    while(s.size() > cnt) s.pop_back();
    return;
}
void Judger::CheckAnswer( RunConfig* result) {
    FILE * out = fopen(config.OutputDataPath.c_str(), "r");
    FILE * ans = fopen(config.StdAnswerPath.c_str(), "r");
    if (out == NULL || ans == NULL) {
        *result = CreateSystemError("Fail to open data file.");
        return;
    }
    char ch;
	ansbuf.clear(); outbuf.clear();
    while( (ch = fgetc(ans) )!=EOF )  {
        ansbuf.push_back(ch);
        if (ansbuf.size() > MAXBUF) {
            *result = CreateSystemError("Answer Data too large.");
            return;
        }
    }
    while( (ch = fgetc(out) )!=EOF )  {
        outbuf.push_back(ch);
        if (outbuf.size() > ansbuf.size() * MAX_OVERFLOW_MULTIPLE) {
            result->Result = JudgerResult::OUTPUT_LIMIT_EXCEEDED;
            result->ResultDetail = "output too large.";
            return;
        }
    }
    if (*(outbuf.rbegin()) == '\n') outbuf.pop_back();
    if (*(ansbuf.rbegin()) == '\n') ansbuf.pop_back();
    if (StrictMode) {
        if (outbuf == ansbuf) {
            result->Result = JudgerResult::ACCEPTED;
            result->ResultDetail = "Ok.";
        } else {
            FormatString(ansbuf); FormatString(outbuf);
            if ( ansbuf!=outbuf ) {
                result->Result = JudgerResult::WRONG_ANSWER;
                result->ResultDetail = "Output is different from std answer after ignore space characters.";
            } else {
                result->Result = JudgerResult::PRESENTATION_ERROR;
                result->ResultDetail = "Output is accepted! but presentation error.";
            }
        }
    } else {
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
    if (runner_id<0) {
        *result = CreateSystemError("Fail to fork runner process.");
        return;
    }
    if (runner_id == 0) {
        ExecRunner(RunnerExePath, task.ExePath, task.InputDataPath, task.OutputDataPath);
        *result = CreateSystemError("Fail to execute runner process.");
        exit(EXIT_FAILURE);
    } else {
        killer_id = fork();
        if (killer_id < 0) {
            *result = CreateSystemError("Fail to fork killer process.");
            return;
        }
        
        if (killer_id == 0) {
            ExecKiller(KillerExePath, runner_id, task.MaxCPUTime);
            *result = CreateSystemError("Fail to execute killer process.");
            exit(EXIT_FAILURE);
        } else {
            cout << killer_id<<" "<< runner_id << endl;
            int killer_status, runner_status;
            struct rusage resource_usage, resource_usage2;
            if (wait4(runner_id, &runner_status, WSTOPPED, &resource_usage) == -1) {
                 kill(runner_id, SIGKILL);
                    *result = CreateSystemError("Fail to wait runner process.");
                    return;
            }
            
            kill(killer_id, SIGKILL);
            if (wait4(killer_id, &killer_status, WSTOPPED, &resource_usage2) == -1) {
                 kill(killer_id, SIGKILL);
                    *result = CreateSystemError("Fail to wait killer process.");
                    return;
            }
            

            GetResult(result, runner_status, resource_usage);
            if (result->Result != JudgerResult::ACCEPTED) {
                return;
            }
            if (config.isSPJ == false) {
                CheckAnswer(result);
            } else {

            }
        }
    }

    return;
}

#include <iostream>


int main(int argc, char * argv[]) {
    
    JudgeConfig config;
    RunConfig result;
    config.MaxCPUTime=1000;
    config.MaxMemory=1000000000;
    Judger::run(config, &result, "runner", "killer");
    freopen("1.txt", "w", stdout);
    cout << result.UseCPUTime << "ms" << endl;
    cout << result.UseMemory << endl;
    cout << result.Result << endl;
    cout << result.ResultDetail << endl;
    return 0;
}
