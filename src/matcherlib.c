#include "judger_config.h"
#include "loglib.h"
#include "matcherlib.h"
#include "boxlib.h"
#include "killerlib.h"

JudgeResult formatMatch(const char *std,  const char *out);
void checkSPJ(const JudgeConfig *config, RunConfig * result, int status, const struct rusage * resourceUsage);


JudgeResult formatMatch(const char *std,  const char *out) {
    while(*std != '\0' && *out != '\0') {
        char chStd, chOut;
        if (!isspace(*std))  {
            chStd = *std;
            std++;
        }
        else {
            chStd = ' ';
            while(isspace(*std)) std++;
        }

        if (!isspace(*out)) {
            chOut = *out;
            out++;
        }
        else {
            chOut = ' ';
            while(isspace(*out)) out++;
        }

        if (chStd != chOut) return WRONG_ANSWER;
    }
    if (*std != *out) return WRONG_ANSWER;
    return ACCEPTED;
}
void matchResult(const JudgeConfig * config, int curCase, RunConfig * result) {
    
#ifdef __DEBUG
    char buffer[MAX_DEBUG_INFO_LENGTH];
    sprintf(buffer, "Matching output and std output");
    logDebugInfoWithMessage(config->logPath, buffer);
#endif

    FILE *stdFile = NULL;
    FILE *outFile = NULL;
    
    stdFile = fopen(config->stdAnswer[curCase], "r");
    
    if (config->iOMode == STD_IO) {
        outFile = fopen(config->outputData[curCase], "r");
    } else {
        outFile = fopen(FILEIO_INPUT_PATH, "r");
    }
    
    if (stdFile == NULL || outFile == NULL) {
        createSystemError(result, MATCHER_OPEN_DATA_FAILED, "Can't open data file", config->logPath);
        return;
    }
    
    char *stdbuf = (char * ) malloc(sizeof(char *) * config->maxCharBuffer);
    char *outbuf = (char * ) malloc(sizeof(char *) * config->maxCharBuffer);
    
    char ch;
    int stdCnt = 0, outCnt = 0;

    stdbuf[stdCnt++] = ' ';
    outbuf[outCnt++] = ' ';
    
    while((ch = fgetc(stdFile)) != EOF) {
        stdbuf[stdCnt++] = ch;
        if (stdCnt>config->maxCharBuffer) {
            createSystemError(result, MATCHER_STD_DATA_TOO_LARGE,"The std answer is too large", config->logPath);
            return;
        }

        /*format the file str for windows input data*/
        if (stdCnt >= 1) {
            if (stdbuf[stdCnt-1] == '\n' && stdbuf[stdCnt-2] == '\r') {
                stdCnt--;
                stdbuf[stdCnt-1] = '\n';
            }
        }

    }
    stdbuf[stdCnt] = '\0';

    while((ch = fgetc(outFile)) != EOF) {
        outbuf[outCnt++] = ch;
        if (outCnt > config->maxCharBuffer) {
            result->result = OUTPUT_LIMIT_EXCEEDED;
            return;
        }

        /*format the file str for windows input data*/
        if (outCnt >= 1) {
            if (outbuf[outCnt-1] == '\n' && outbuf[outCnt-2] == '\r') {
                outCnt--;
                outbuf[outCnt-1] = '\n';
            }
        }
        
    }
    outbuf[outCnt] = '\0';
    
    fclose(stdFile); fclose(outFile);

    if (outbuf[outCnt-1] == '\n') outbuf[--outCnt] = '\0';
    if (stdbuf[stdCnt-1] == '\n') stdbuf[--stdCnt] = '\0';

    if (config->strictMode == STRICT_MODE) {
        
        if (outCnt==stdCnt && strcmp(outbuf, stdbuf) == 0) {
            result->result = ACCEPTED;
            return;
        }
    }
    
    while(stdCnt > 0 && isspace(stdbuf[stdCnt-1])) stdCnt--;
    stdbuf[stdCnt] = '\0';
    while(outCnt > 0 && isspace(outbuf[outCnt-1])) outCnt--;
    outbuf[outCnt] = '\0';
    result->result = formatMatch(stdbuf, outbuf);
    
    if (result->result == ACCEPTED && config->strictMode == STRICT_MODE) result->result = PRESENTATION_ERROR;
    
    return;
}
void checkSPJ(const JudgeConfig *config, RunConfig * result, int status, const struct rusage * resourceUsage) {



    int exitSignal = WTERMSIG(status);
    if (exitSignal == SIGUSR1) {
        result->result = SYSTEM_ERROR;
        strcpy(result->resultDetail, "Check log file to know detail");
        return ;
    }
    if (exitSignal != 0) {
        result->result = WRONG_ANSWER;
        return;
    }
    
    int exitCode =  WEXITSTATUS(status);
    int useCPUTime = (int) (resourceUsage->ru_utime.tv_sec * 1000 + resourceUsage->ru_utime.tv_usec / 1000);
    long long useMemory = resourceUsage->ru_maxrss * 1024;
#ifdef __DEBUG
    char buffer[MAX_DEBUG_INFO_LENGTH];
    sprintf(buffer, "Box (spj) exit signal : %d, exit code : %d", exitSignal, exitCode);
    logDebugInfoWithMessage(config->logPath, buffer);
#endif

    if (useCPUTime > config->maxSPJTime) {
        result->result = WRONG_ANSWER;
        return;
    }
    if (useMemory > config->maxSPJMemory) {
        result->result = WRONG_ANSWER;
        return;
    }
    if (exitCode == ACCEPTED)
        result->result = ACCEPTED;
    else if (exitCode == WRONG_ANSWER) {
        result->result = WRONG_ANSWER;
    } else if (exitCode == 2) {
        createSystemError(result, MATCHER_SPJ_CAN_NOT_OPEN_DATA_FILE,"Spj can't open data file", config->logPath);
        return;
    } else if (exitCode == 3) {
        createSystemError(result, MATCHER_SPJ_ARGV_ERROR,"Spj argv error", config->logPath);
        return;
    } else {
        result->result = WRONG_ANSWER;
    }
    return;
}   

void matchWithSPJ(const JudgeConfig *config, int curCase, RunConfig * result) {
#ifdef __DEBUG
    char buffer[MAX_DEBUG_INFO_LENGTH];
    sprintf(buffer, "Running spj to judge");
    logDebugInfoWithMessage(config->logPath, buffer);
#endif

    pid_t spjID = fork();
    if (spjID<0) {
        createSystemError(&result[curCase], FORK_SPJ_FAILED, "Can't fork spj proccess", config->logPath);
        return;
    } else if (spjID == 0) {
        runSpj(config, curCase);
    } else {
        pid_t killerID = fork();
        if (killerID < 0) {
            kill(spjID, SIGKILL);
            createSystemError(&result[curCase], FORK_KILLER_FOR_SPJ_FAILED, "Can't fork killer proccess for spj", config->logPath);
            return;
        } else if (killerID==0) {
            monitor(spjID, config->maxSPJTime);
        } else {
            int status;
            struct rusage resourceUsage;

            if (wait4(spjID, &status, 0, &resourceUsage) == -1) {
                kill(killerID, SIGKILL);
                createSystemError( &result[curCase], WAIT_BOX_FAILED, "Can't wait box proccess", config->logPath);
                return;
            }
            
            kill(killerID, SIGKILL);
            checkSPJ(config, result, status, &resourceUsage);
        }
    }

}