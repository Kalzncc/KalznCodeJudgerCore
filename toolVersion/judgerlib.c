#include "judger_config.h"
#include "loglib.h"
#include "judgerlib.h"
#include "boxlib.h"
#include "killerlib.h"
#include "matcherlib.h"
#include "compilerlib.h"



void initRunConfig(RunConfig * result) {
    result->errorCode = result->exitCode = result->exitSignal = 0;
    result->result = SKIP;
    result->useCPUTime = result->useMemory = 0;
    result->resultDetail = (char * )malloc(sizeof(char) * MAX_DETAIL_LENGTH);
    strcpy(result->resultDetail, "No info");
}
void check(RunConfig *result, int status, const struct rusage *resourceUsage, int timeLimit, long long memoryLimit,  const char * logPath ) {
    
    result->exitSignal = WTERMSIG(status);
    if (result->exitSignal == SIGUSR1) {
        result->result = SYSTEM_ERROR;
        strcpy(result->resultDetail, "Check log file to know detail");
        return ;
    }
    result->exitCode =  WEXITSTATUS(status);
#ifdef __DEBUG
    char buffer[MAX_DEBUG_INFO_LENGTH];
    sprintf(buffer, "Box exit signal : %d, exit code : %d", result->exitSignal, result->exitCode);
    logDebugInfoWithMessage(logPath, buffer);
#endif

    result->useCPUTime = (int) (resourceUsage->ru_utime.tv_sec * 1000 + resourceUsage->ru_utime.tv_usec / 1000);
    result->useMemory = resourceUsage->ru_maxrss * 1024;

    if (result->exitCode != 0) {
        result->result = RUNTIME_ERROR;
        strcpy(result->resultDetail, "No zero return");
        return;
    }
    if (result->exitSignal != 0) {
        result->result = RUNTIME_ERROR;
    }
    if (result->useCPUTime > timeLimit) {
        result->result = TIME_LIMIT_EXCEEDED;
    }
    if (result->useMemory > memoryLimit) {
        result->result = MEMORY_LIMIT_EXCEEDED;
    }
    
}   
RunConfig* judge(const JudgerConfig * judgerConfig, const JudgeConfig *judgeConfig) {
    
    RunConfig *result = (RunConfig*) malloc (sizeof(RunConfig)* judgeConfig->caseNumber );
    int curCase;
#ifdef __DEBUG
    logDebugInfoWithMessage(judgeConfig->logPath, "Initializing result config");
#endif
    for (curCase = 0; curCase < judgeConfig->caseNumber; curCase++) {
        
        initRunConfig(&result[curCase]);
        result[curCase].taskID = judgeConfig->taskID;
    }
#ifdef __DEBUG
    logDebugInfoWithMessage(judgeConfig->logPath, "Checking judger config");
#endif
    if (getuid()!=0) {
        createSystemError(&result[0], PERMISSION_ERROR, "No root", judgeConfig->logPath);
        return result;
    }
    if (chdir(judgeConfig->workSpacePath)!=0) {
        createSystemError(&result[0], ACCESS_WORKSPACE_FAILED, "Can't access workspace.", judgeConfig->logPath);
    }

    if (judgerConfig->maxCharBuffer<1 || judgerConfig->maxCharBuffer>MAX_OUTPUT_CHAR_BUFFER
        || judgerConfig->maxSPJMemory<1 || judgerConfig->maxSPJMemory>MAX_SPJ_MEMORY_LIMIT
        || judgerConfig->maxSPJTime<1 || judgerConfig->maxSPJTime>MAX_SPJ_TIME_LIMIT
        )
    {
        createSystemError(&result[0], INVALID_JUDGE_CONFIG, "Judger limit is invalid", judgeConfig->logPath);
        return result;
    }

    if (judgeConfig->translator.mode != INTERPRETER_MOD) {
#ifdef __DEBUG
    logDebugInfoWithMessage(judgeConfig->logPath, "Compileing code");
#endif
        if (compileCode(judgeConfig->translator.compilerPath, judgeConfig->translator.compilerOptions)) {
            createSystemError(&result[0], COMPILER_RUN_FAILED, "Cant't run compiler", judgeConfig->logPath);
            return result;
        }
        FILE *fp;
        if ( (fp = fopen(judgeConfig->translator.compilerProductName, "r")) == NULL) {
            result[0].result = COMPILE_ERROR;

            strcpy(result[0].resultDetail, "Code compile failed");
            return result;
        } else {
            fclose(fp);
        }
    }
    

    for (curCase = 0; curCase < judgeConfig->caseNumber; curCase++) {
#ifdef __DEBUG
    char buffer[MAX_DEBUG_INFO_LENGTH];
    sprintf(buffer, "Begin to judge data %d", curCase);
    logDebugInfoWithMessage(judgeConfig->logPath, buffer);
#endif
        if (judgeConfig->maxCPUTime[curCase]<1 || judgeConfig->maxCPUTime[curCase] > MAX_TIME_LIMIT
        || judgeConfig->maxMemory[curCase]<1 || judgeConfig->maxMemory[curCase] > MAX_MEMORY_LIMIT
        || judgeConfig->maxStack[curCase]<1 || judgeConfig->maxStack[curCase]> MAX_STACK_LIMIT
        )
        {
            createSystemError( &result[curCase], INVALID_JUDGE_CONFIG, "Resource limit is invalid", judgeConfig->logPath);
        }
        if (judgeConfig->iOMode == FILE_IO) {
            FILE *fpTar, *fpOr;
            if ((fpTar = fopen(FILEIO_INPUT_PATH, "w")) == NULL || (fpOr = fopen(judgeConfig->inputData[curCase], "r") )== NULL) {
                createSystemError(&result[curCase], FILE_IO_INIT_FAILED, "Can't copy file in File IO Mode", judgeConfig->logPath);
                return result;
            }
            char ch;
            while((ch = fgetc(fpOr)) != EOF) fputc(ch, fpTar);
            if (fclose(fpTar)!=0 || fclose(fpOr) != 0) {
                createSystemError(&result[curCase], FILE_IO_INIT_FAILED, "Can't close file in File IO Mode", judgeConfig->logPath);
                return result;
            }
        }
        pid_t boxID = fork();
        if (boxID<0) {
            createSystemError(&result[curCase], FORK_BOX_FAILED, "Can't fork box proccess", judgeConfig->logPath);
            return result;
        } else if (boxID == 0) {
            run(judgerConfig, judgeConfig, curCase);
        } else {
            pid_t killerID = fork();
            if (killerID < 0) {
                kill(boxID, SIGKILL);
                createSystemError(&result[curCase], FORK_KILLER_FAILED, "Can't fork killer proccess", judgeConfig->logPath);
                return result;
            } else if (killerID==0) {
                monitor(boxID, judgeConfig->maxCPUTime[curCase]);
            } else {
                
                int status;
                struct rusage resourceuUsage;

                if (wait4(boxID, &status, WSTOPPED, &resourceuUsage) == -1) {
                    kill(killerID, SIGKILL);
                    createSystemError( &result[curCase], WAIT_BOX_FAILED, "Can't wait box proccess", judgeConfig->logPath);
                    return result;
                }
                kill(killerID, SIGKILL);
                
                check(&result[curCase], status, &resourceuUsage,  judgeConfig->maxCPUTime[curCase], judgeConfig->maxMemory[curCase],  judgeConfig->logPath);
                if (result[curCase].result == SYSTEM_ERROR) return result;
                
                if (result[curCase].result == SKIP) {
                    if (judgeConfig->isSPJ == 0) {
                        
                        matchResult(judgerConfig, judgeConfig, curCase, &result[curCase]);
                        
                    } else {
                        matchWithSPJ(judgerConfig, judgeConfig, curCase, &result[curCase], judgeConfig->logPath);
                    }
                }
            }
        }
        if (result[curCase].result != ACCEPTED && judgeConfig->judgeMode == SINGLE_RESULT_MODE) {
            return result;
        }
    }
    
    return result;

}