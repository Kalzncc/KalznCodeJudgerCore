#ifndef JUDGERJSON_H
#define JUDGERJSON_H

#include "cjson/cJSON.h"
#include "cjson/cJSON_Utils.h"
#include "judger_config.h"
#include "loglib.h"

int readNonNegativeIntJson(cJSON *cjson, const char *key, int *val, const int defaultVal);
int readStringJson(cJSON *cjson, const char *key, char *val, const char *defaultVal);
int readBoolJson(cJSON *cjson, const char *key, int *val, const int defaultVal);
int loadJudgeConfig(const char *jsonStr, JudgeConfig *config, char *logPath);
int createResultJson(struct timeval startTime, const JudgeConfig *config, const RunConfig *result, cJSON *resultJson);


#endif