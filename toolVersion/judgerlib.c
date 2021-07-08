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
}
void check(RunConfig *result, int status, const struct rusage *resourceUsage, int timeLimit, long long memoryLimit,  const char * logPath ) {
    
    result->exitSignal = WTERMSIG(status);
    if (result->exitSignal == BOX_DATA_REDIRECT_FAILED) {
        createSystemError(result, BOX_DATA_REDIRECT_FAILED, "Can't redirect data file", logPath);
        return ;
    }
    if (result->exitSignal == BOX_EXE_RUN_FAILED) {
        createSystemError(result, BOX_DATA_REDIRECT_FAILED, "Can't run pending pending file/interpreter", logPath);
        return;
    }
    if (result->exitSignal == BOX_SECURITY_CONFIG_LOAD_FAILED) {
        createSystemError(result, BOX_SECURITY_CONFIG_LOAD_FAILED, "Load security config failed", logPath);
        return;
    }
    if (result->exitSignal == BOX_SET_LIMIT_FAILED) {
        createSystemError(result, BOX_SET_LIMIT_FAILED, "Set resource limit failed", logPath);
        return;
    }
    if (result->exitSignal == BOX_SET_UID_FATLED) {
        createSystemError(result, BOX_SET_UID_FATLED, "Set uid/gid failed", logPath);
        return;
    }
    result->exitCode =  WEXITSTATUS(status);
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
    
    RunConfig *result = (RunConfig*) malloc (sizeof(RunConfig*)*judgeConfig->caseNumber );
    int curCase;
    for (curCase = 0; curCase < judgeConfig->caseNumber; curCase++) {
        initRunConfig(&result[curCase]);
        result[curCase].taskID = judgeConfig->taskID;
    }
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
        if (compileCode(judgeConfig->translator.compilerPath, judgeConfig->translator.compilerOptions)) {
            createSystemError(&result[0], COMPILER_RUN_FAILED, "Cant't run compiler", judgeConfig->logPath);
            return result;
        }
        
        FILE *fp;
        if ( (fp = fopen(judgeConfig->translator.compilerProductName, "r")) == NULL) {
            result[0].result = COMPILE_ERROR;
            return result;
        } else {
            fclose(fp);
        }
    }
    
/*
    for (curCase = 0; curCase < judgeConfig->caseNumber; curCase++) {
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

                check(&result[curCase], status, &resourceuUsage,  judgeConfig->maxCPUTime[curCase], judgeConfig->maxCPUTime[curCase],  judgeConfig->logPath);
                if (result[curCase].result == SYSTEM_ERROR) return result;

                if (result[curCase].result == ACCEPTED) {
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
    */
    return result;

}