#include "judger_config.h"
#include "loglib.h"


void createSystemError(RunConfig * result, JudgeError error, const char * message, const char * logPath) {
    result->errorCode = error;
    result->result = SYSTEM_ERROR;
    strcpy(result->resultDetail, message);
    logSystemErrorWithResult(logPath, result);
}

#ifdef __DEBUG
void logDebugInfoWithMessage(const char *path, const char *message) {
    
    FILE * logFile = fopen(path, "a+");
    if (logFile == NULL) {
        fprintf(stderr, "Error: Can't open log file.");
        return;
    }
    static char buffer[MAX_LOG_LENGTH];
    static char datetime[MAX_LOG_LENGTH];
    static time_t nowTime;
    nowTime = time(NULL);
    strftime(datetime, MAX_LOG_LENGTH-1, "%Y-%m-%d %H:%M:%S", localtime(&nowTime));
    int cnt = snprintf(buffer, MAX_LOG_LENGTH, "DEBUG: [%s] %s (Get from message).\n", datetime, message);
    int logFd = fileno((FILE *) logFile);
    if (flock(logFd, LOCK_EX) == 0) {
        if (write(logFd, buffer, (size_t)cnt) < 0) {
            fprintf(stderr, "Error Can't write log file");
        }
        flock(logFd, LOCK_UN);
    } else {
        fprintf(stderr, "Error : Can't flock log file");
    }
    
    fclose(logFile);
    fprintf(stderr, "%s", buffer);
}
#endif
void logSystemErrorWithResult(const char * path, const RunConfig* result) {
    FILE * logFile = fopen(path, "a+");
    
    if (logFile == NULL) {
        fprintf(stderr, "Error: Can't open log file.");
        return;
    }
    static char buffer[MAX_LOG_LENGTH];
    static char datetime[MAX_LOG_LENGTH];
    static time_t nowTime;
    nowTime = time(NULL);
    strftime(datetime, MAX_LOG_LENGTH-1, "%Y-%m-%d %H:%M:%S", localtime(&nowTime));
    int cnt = snprintf(buffer, MAX_LOG_LENGTH, "ERROR:%d [%s] %s (Get from judger in taskID:%lld; detail: useCPUTime:%d, useMemory:%lld, exitSignal:%d, exitCode:%d).\n", result->errorCode, datetime, result->resultDetail, result->taskID, result->useCPUTime,  result->useMemory, result->exitSignal, result->exitCode);
    int logFd = fileno((FILE *) logFile);
    if (flock(logFd, LOCK_EX) == 0) {
        if (write(logFd, buffer, (size_t)cnt) < 0) {
            fprintf(stderr, "Error Can't write log file");
        }
        flock(logFd, LOCK_UN);
    } else {
        fprintf(stderr, "Error : Can't flock log file");
    }
    fclose(logFile);
#ifdef __DEBUG
    fprintf(stderr, "%s", buffer);
#endif
}
void logSystemErrorWithMessage(const char * path, JudgeError error, const char * message) {
    FILE * logFile = fopen(path, "a+");
    if (logFile == NULL) {
        fprintf(stderr, "Error: Can't open log file.");
        return;
    }
    static char buffer[MAX_LOG_LENGTH];
    static char datetime[MAX_LOG_LENGTH];
    static time_t nowTime;
    nowTime = time(NULL);
    strftime(datetime, MAX_LOG_LENGTH-1, "%Y-%m-%d %H:%M:%S", localtime(&nowTime));
    int cnt = snprintf(buffer, MAX_LOG_LENGTH, "ERROR:%d [%s] %s (Get from message).\n",error, datetime, message);
    int logFd = fileno((FILE *) logFile);
    if (flock(logFd, LOCK_EX) == 0) {
        if (write(logFd, buffer, (size_t)cnt) < 0) {
            fprintf(stderr, "Error Can't write log file");
        }
        flock(logFd, LOCK_UN);
    } else {
        fprintf(stderr, "Error : Can't flock log file");
    }
    fclose(logFile);
#ifdef __DEBUG
    fprintf(stderr, "%s", buffer);
#endif
}
void logSystemErrorWithTaskID(const char * path, int taskID, JudgeError error, const char * message) {
    FILE * logFile = fopen(path, "a+");
    if (logFile == NULL) {
        fprintf(stderr, "Error: Can't open log file.");
        return;
    }
    static char buffer[MAX_LOG_LENGTH];
    static char datetime[MAX_LOG_LENGTH];
    static time_t nowTime;
    nowTime = time(NULL);
    strftime(datetime, MAX_LOG_LENGTH-1, "%Y-%m-%d %H:%M:%S", localtime(&nowTime));
    int cnt = snprintf(buffer, MAX_LOG_LENGTH, "ERROR:%d [%s] %s (Get from taskID : %d).\n",error, datetime, message, taskID);
    int logFd = fileno((FILE *) logFile);
    if (flock(logFd, LOCK_EX) == 0) {
        if (write(logFd, buffer, (size_t)cnt) < 0) {
            fprintf(stderr, "Error Can't write log file");
        }
        flock(logFd, LOCK_UN);
    } else {
        fprintf(stderr, "Error : Can't flock log file");
    }
    fclose(logFile);
#ifdef __DEBUG
    fprintf(stderr, "%s", buffer);
#endif
}

