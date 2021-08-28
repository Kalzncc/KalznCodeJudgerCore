#include "judger_config.h"
#include "loglib.h"
#include "judgerlib.h"
#include "boxlib.h"
#include "killerlib.h"
#include "matcherlib.h"
#include "compilerlib.h"

void initRunConfig(RunConfig * result);
void check(RunConfig *result, int status, const struct rusage *resourceUsage, int timeLimit, long long memoryLimit,  const char * logPath );


void initRunConfig(RunConfig * result) {
    result->errorCode = result->exitCode = result->exitSignal = 0;
    result->result = SKIP;
    result->useCPUTime = result->useMemory = 0;
    result->useRealTime = 0;
    result->resultDetail = (char * )malloc(sizeof(char) * MAX_DETAIL_LENGTH);
    strcpy(result->resultDetail, "No info");
}
void check(RunConfig *result, int status, const struct rusage *resourceUsage, int timeLimit, long long memoryLimit,  const char * logPath ) {
    
    result->exitSignal = WTERMSIG(status);
    if (result->exitSignal == SIGUSR1) {
        result->result = SYSTEM_ERROR;
        strcpy(result->resultDetail, "Access log file to check detail");
        return ;
    }
    result->exitCode =  WEXITSTATUS(status);
#ifdef __DEBUG
    char buffer[MAX_DEBUG_INFO_LENGTH];
    sprintf(buffer, "Box exit signal : %d, exit code : %d", result->exitSignal, result->exitCode);
    logDebugInfoWithMessage(logPath, buffer);
#endif

    result->useCPUTime = (int) (resourceUsage->ru_utime.tv_sec * 1000 + resourceUsage->ru_utime.tv_usec / 1000);
    result->useMemory = resourceUsage->ru_maxrss;
    
    if (result->exitCode != 0) {
        result->result = RUNTIME_ERROR;
        strcpy(result->resultDetail, "No zero return");
        return;
    }
    if (result->exitSignal != 0) {
        result->result = RUNTIME_ERROR;
    }
    if (result->useMemory > memoryLimit) {
        result->result = MEMORY_LIMIT_EXCEEDED;
    }
    if (result->useCPUTime > timeLimit) {
        result->result = TIME_LIMIT_EXCEEDED;
    }
    
    
}   
RunConfig* judge(const JudgeConfig *config) {
    RunConfig *result = NULL;
    result = (RunConfig*) malloc (sizeof(RunConfig)* config->caseNumber );
    int curCase;
#ifdef __DEBUG
    logDebugInfoWithMessage(config->logPath, "Initializing result config");
#endif
    for (curCase = 0; curCase < config->caseNumber; curCase++) {
        initRunConfig(&result[curCase]);
        result[curCase].taskID = config->taskID;
    }
#ifdef __DEBUG
    logDebugInfoWithMessage(config->logPath, "Checking judger config");
#endif
    if (getuid()!=0) {
        createSystemError(&result[0], PERMISSION_ERROR, "No root", config->logPath);
        return result;
    }
    if (chdir(config->workSpacePath)!=0) {
        createSystemError(&result[0], ACCESS_WORKSPACE_FAILED, "Can't access workspace.", config->logPath);
        return result;
    }

    if (config->maxCharBuffer<1 || config->maxCharBuffer>MAX_OUTPUT_CHAR_BUFFER
        || (config->isSPJ == 1 && (config->maxSPJMemory<1 || config->maxSPJMemory>MAX_SPJ_MEMORY_LIMIT || config->maxSPJTime<1 || config->maxSPJTime>MAX_SPJ_TIME_LIMIT) )
        )
    {
        createSystemError(&result[0], INVALID_JUDGE_CONFIG, "Judger limit is invalid", config->logPath);
        return result;
    }

    if (config->translator.mode != INTERPRETER_MODE && config->translator.mode != DO_NOT_TANSLATE_MODE) {
#ifdef __DEBUG
    logDebugInfoWithMessage(config->logPath, "Compileing code");
#endif
        remove(config->translator.compilerProductName);
        if (compileCode(config->translator.compilerPath, config->translator.compilerInfoPath, config->translator.compilerOptions)) {
            createSystemError(&result[0], COMPILER_RUN_FAILED, "Cant't run compiler", config->logPath);
            return result;
        }
        FILE *fp;
        if ( (fp = fopen(config->translator.compilerProductName, "r")) == NULL) {
            result[0].result = COMPILE_ERROR;

            strcpy(result[0].resultDetail, "Code compile failed");
            return result;
        } else {
            fclose(fp);
        }
    }
    if (config->judgeMode == ONLY_COMPILE_MODE) {
        result->result = ACCEPTED;
        return result;
    }

    for (curCase = 0; curCase < config->caseNumber; curCase++) {
#ifdef __DEBUG
    char buffer[MAX_DEBUG_INFO_LENGTH];
    sprintf(buffer, "Begin to judge data %d", curCase);
    logDebugInfoWithMessage(config->logPath, buffer);
#endif
        if (config->maxCPUTime[curCase]<1 || config->maxCPUTime[curCase] > MAX_TIME_LIMIT
        || config->maxMemory[curCase]<1 || config->maxMemory[curCase] > MAX_MEMORY_LIMIT
        || config->maxStack[curCase]<1 || config->maxStack[curCase]> MAX_STACK_LIMIT
        )
        {
            createSystemError( &result[curCase], INVALID_JUDGE_CONFIG, "Resource limit is invalid", config->logPath);
        }
        if (config->iOMode == FILE_IO) {
            FILE *fpTar, *fpOr;
            if ((fpTar = fopen(FILEIO_INPUT_PATH, "w")) == NULL || (fpOr = fopen(config->inputData[curCase], "r") )== NULL) {
                createSystemError(&result[curCase], FILE_IO_INIT_FAILED, "Can't copy file in File IO Mode", config->logPath);
                return result;
            }
            char ch;
            while((ch = fgetc(fpOr)) != EOF) fputc(ch, fpTar);
            if (fclose(fpTar)!=0 || fclose(fpOr) != 0) {
                createSystemError(&result[curCase], FILE_IO_INIT_FAILED, "Can't close file in File IO Mode", config->logPath);
                return result;
            }
        }
        pid_t boxID = fork();
        if (boxID<0) {
            createSystemError(&result[curCase], FORK_BOX_FAILED, "Can't fork box proccess", config->logPath);
            return result;
        } else if (boxID == 0) {
            run(config, curCase);
        } else {
            
            pid_t killerID = fork();
            if (killerID < 0) {
                kill(boxID, SIGKILL);
                createSystemError(&result[curCase], FORK_KILLER_FAILED, "Can't fork killer proccess", config->logPath);
                return result;
            } else if (killerID==0) {
                monitor(boxID, config->maxCPUTime[curCase]);
            } else {
                
                int status;
                struct rusage resourceuUsage;
                struct timeval boxStartTime, boxExitTime;
                gettimeofday(&boxStartTime, NULL);
                if (wait4(boxID, &status, 0, &resourceuUsage) == -1) {
                    kill(killerID, SIGKILL);
                    createSystemError( &result[curCase], WAIT_BOX_FAILED, "Can't wait box proccess", config->logPath);
                    return result;
                }
                gettimeofday(&boxExitTime, NULL);
                result[curCase].useRealTime = (boxExitTime.tv_sec * 1000 + boxExitTime.tv_usec / 1000 - (boxStartTime.tv_sec * 1000 + boxStartTime.tv_usec / 1000));
                
                kill(killerID, SIGKILL);
                
                check(&result[curCase], status, &resourceuUsage,  config->maxCPUTime[curCase], config->maxMemory[curCase],  config->logPath);
                if (result[curCase].result == SYSTEM_ERROR) return result;
                
                if (result[curCase].result == SKIP) {
                    if (config->isSPJ == 0) {
                        matchResult(config, curCase, &result[curCase]);
                        
                    } else {
                        matchWithSPJ(config,  curCase, &result[curCase]);
                    }
                }
            }
        }
        if (result[curCase].result != ACCEPTED && result[curCase].result != PRESENTATION_ERROR && config->judgeMode == SINGLE_RESULT_MODE) {
            return result;
        }
    }
    
    return result;

}