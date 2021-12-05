#ifndef LOGLIB_H
#define LOGLIB_H

#define MAX_LOG_LENGTH 1024

#include "judger_config.h"


void logDebugInfoWithMessage(const char *path, const char *message);
void logSystemErrorWithResult(const char *path, const RunConfig* result);
void logSystemErrorWithMessage(const char *path, JudgeError error, const char *message);
void logSystemErrorWithTaskID(const char *path, const char *taskID, JudgeError error, const char *message);
void createSystemError(RunConfig *result, JudgeError error, const char *message, const char *logPath);

#endif