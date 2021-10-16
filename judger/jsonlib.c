#include "jsonlib.h"

int readNonNegativeIntJson(cJSON *cjson, const char * key, int *val, const int defaultVal) {
    cJSON *temp = cJSON_GetObjectItem(cjson, key);
    if (temp == NULL || !cJSON_IsNumber(temp) || cJSON_GetNumberValue(temp) < 0) {
        if (defaultVal >= 0){
            *val = defaultVal;
            return 0;
        }
        return 1;
    }
    *val = temp->valueint;
    return 0;
}
int readStringJson(cJSON *cjson, const char *key, char *val, const char *defaultVal) {
    cJSON *temp = cJSON_GetObjectItem(cjson, key);
    if ( temp == NULL || !cJSON_IsString(temp) ) {
        if (defaultVal != NULL) {
            strcpy(val, defaultVal);
            return 0;       
        }
        return 1;
    }
    strcpy(val, temp->valuestring);
    return 0;
}
int readBoolJson(cJSON * cjson, const char *key, int *val, const int defaultVal) {
    cJSON *temp = cJSON_GetObjectItem(cjson, key);
    if (temp == NULL || !cJSON_IsBool(temp)) {
        if (defaultVal >= 0){
            *val = defaultVal;
            return 0;
        }
        return 1;
    }
    *val = temp->valueint;
    return 0;
}
int loadJudgeConfig(const char *jsonStr, JudgeConfig *config, char *logPath) {
    cJSON *cjson;
    int num; 

    cjson = cJSON_Parse(jsonStr);

    char *stringVal = (char *)malloc(sizeof(char)*strlen(jsonStr));
    readStringJson(cjson, "logPath", stringVal, logPath);
    config->logPath = (char *)malloc(sizeof(char) * strlen(stringVal));
    strcpy(config->logPath, stringVal);
    strcpy(logPath, config->logPath);

    if (readNonNegativeIntJson(cjson, "maxCharBuffer", &(config->maxCharBuffer), -1)) {
        logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "Necessary attribute not is found or invalid : maxCharBuffer");
        return 1;
    }
    
    if (readStringJson(cjson, "taskID", stringVal, NULL)) {
        logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "Necessary attribute not is found or invalid : taskID");
        return 1;
    }
    config->taskID = atol(stringVal);
    if (config->taskID < 1) {
        logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "Necessary attribute not is found or invalid : taskID");
        return 1;
    }
    
    readNonNegativeIntJson(cjson, "uid", &(config->uid), DEFAULT_UID);
    readNonNegativeIntJson(cjson, "gid", &(config->gid), DEFAULT_UID);
    readNonNegativeIntJson(cjson, "judgeMode", &(config->judgeMode), 0);
    readNonNegativeIntJson(cjson, "iOMode", &(config->iOMode), 0);
    readNonNegativeIntJson(cjson, "strictMode",&(config->strictMode), NOT_STRICT_MODE);

    if (readStringJson(cjson, "workSpacePath", stringVal, NULL)) {
        logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "Necessary attribute not is found or invalid : workSpacePath");
        return 1;
    }
    config->workSpacePath = (char*)malloc(sizeof(char) * strlen(stringVal));
    strcpy(config->workSpacePath, stringVal);

    readStringJson(cjson, "resultPath", stringVal, DEFAULT_RESULT_FILE_PATH);
    config->resultPath = (char*)malloc(sizeof(char) * strlen(stringVal));
    strcpy(config->resultPath, stringVal);

    readBoolJson(cjson, "sPJ", &(config->isSPJ), 0);
 
    if (config->isSPJ != 0) {
        readNonNegativeIntJson(cjson, "maxSPJTime", &(config->maxSPJTime), MAX_SPJ_TIME_LIMIT);
        readNonNegativeIntJson(cjson, "maxSPJMemory", &(config->maxSPJTime), MAX_SPJ_MEMORY_LIMIT);
        if (readStringJson(cjson, "spjExePath", stringVal, NULL)) {
            logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "Necessary attribute not is found or invalid : spjExePath");
            return 1;
        }
        config->sPJPath = (char*)malloc(sizeof(char) * strlen(stringVal));
        strcpy(config->sPJPath, stringVal);

        if (readStringJson(cjson, "spjExeName", stringVal, NULL)) {
            logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "Necessary attribute not is found or invalid : spjExeName");
            return 1;
        }
        config->sPJName = (char*)malloc(sizeof(char) * strlen(stringVal));
        strcpy(config->sPJName, stringVal);
    }

    cJSON *transJson = cJSON_GetObjectItem(cjson, "translator");
    if (transJson == NULL) {
        logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "Necessary attribute not is found or invalid : translator");
        return 1;
    }
    readNonNegativeIntJson(transJson, "mode",&(config->translator.mode), COMPILER_MODE);

    if (config->translator.mode == COMPILER_MODE || config->translator.mode == COMPILER_INTERPRETER_MODE) {
        if (readStringJson(transJson, "compilerPath", stringVal, NULL)) {
            logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "Necessary attribute not is found or invalid : compilerPath");
            return 1;
        }
        config->translator.compilerPath = (char*)malloc(sizeof(char) * strlen(stringVal));
        strcpy(config->translator.compilerPath, stringVal);

        cJSON *temp;
        if ( ( temp = cJSON_GetObjectItem(transJson, "compilerOptions") ) == NULL || !cJSON_IsArray(temp)) {
            logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "Necessary attribute not is found or invalid : compilerOptions"); 
            return 1;
        } else {
            int compilerOptionCnt = cJSON_GetArraySize(temp);
            config->translator.compilerOptions = (char **)malloc(sizeof(char*) * compilerOptionCnt+1);
            config->translator.compilerOptions[compilerOptionCnt] = NULL;
            int i;
            for (i = 0; i < compilerOptionCnt; i++) {
                if ( !cJSON_IsString(cJSON_GetArrayItem(temp, i)) ) {
                    logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "Necessary attribute not is found or invalid : compilerOptions");
                    return 1;
                }
                config->translator.compilerOptions[i] = (char * )malloc((strlen(cJSON_GetArrayItem(temp, i)->valuestring)+1)*sizeof(char));
                strcpy(config->translator.compilerOptions[i], cJSON_GetArrayItem(temp, i)->valuestring);
            } 
        }
        readStringJson(transJson, "compilerInfoPath", stringVal, DEFAULT_COMPILER_INFO_PATH);
        config->translator.compilerInfoPath = (char*)malloc(sizeof(char) * strlen(stringVal));
        strcpy(config->translator.compilerInfoPath, stringVal);
    }
    if (config->translator.mode != INTERPRETER_MODE) {
        if (readStringJson(transJson, "compilerProductName", stringVal, NULL)) {
            logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "Necessary attribute not is found or invalid : compilerProductName");
            return 1;
        }
        config->translator.compilerProductName = (char*)malloc(sizeof(char) * strlen(stringVal));
        strcpy(config->translator.compilerProductName, stringVal);
    }

    if (config->translator.mode == INTERPRETER_MODE || config->translator.mode == COMPILER_INTERPRETER_MODE) {
        if (readStringJson(transJson, "interpreterPath",stringVal, NULL)) {
            logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "Necessary attribute not is found or invalid : interpreterPath");
            return 1;
        }
        config->translator.interpreterPath = (char*)malloc(sizeof(char) * strlen(stringVal));
        strcpy(config->translator.interpreterPath, stringVal);

        cJSON *temp;
        if ( ( temp = cJSON_GetObjectItem(transJson, "interpreterOptions") ) == NULL || !cJSON_IsArray(temp)) {
            logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "Necessary attribute not is found or invalid : interpreterOptions");
            return 1;
        } else {
            int interpreterOptionCnt = cJSON_GetArraySize(temp);
            config->translator.interpreterOptions = (char **)malloc(sizeof(char*) * interpreterOptionCnt+1);
            config->translator.interpreterOptions[interpreterOptionCnt] = NULL;
            int i;
            for (i = 0; i < interpreterOptionCnt; i++) {
                if ( !cJSON_IsString(cJSON_GetArrayItem(temp, i)) ) {
                    logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "Necessary attribute not is found or invalid : interpreterOptions");
                    return 1;                     
                }
                config->translator.interpreterOptions[i] = (char * )malloc((strlen(cJSON_GetArrayItem(temp, i)->valuestring)+1)*sizeof(char));
                strcpy(config->translator.interpreterOptions[i], cJSON_GetArrayItem(temp, i)->valuestring);
            }
             
        }
        readStringJson(transJson, "interpreterInfoPath", stringVal, DEFAULT_INTERPRETER_INFO_PATH);
        config->translator.interpreterInfoPath = (char*)malloc(sizeof(char) * strlen(stringVal));
        strcpy(config->translator.interpreterInfoPath, stringVal);
    }
    cJSON *dataJson = cJSON_GetObjectItem(cjson, "data");
    if (!cJSON_IsArray(dataJson)) {
        free(dataJson);
        dataJson = NULL;
    }
    int dataCnt = cJSON_GetArraySize(dataJson);
    if (dataCnt > MAX_DATA_CASE_NUMBER) {
        logSystemErrorWithMessage(logPath, INVALID_JUDGE_CONFIG, "Data case too many");
        return 1;
    }
    if (dataCnt == 0 && config->judgeMode != ONLY_COMPILE_MODE ) {
        logSystemErrorWithMessage(logPath, INVALID_JUDGE_CONFIG, "data config invalid");
        return 1;
    }
    if (config->judgeMode == ONLY_COMPILE_MODE) dataCnt = 1;
    config->inputData = (char **) malloc(sizeof(char *) * dataCnt);
    config->outputData = (char **) malloc(sizeof(char *) * dataCnt);
    config->stdAnswer = (char **) malloc(sizeof(char *) * dataCnt);
    config->maxCPUTime = (int *) malloc(sizeof(int) * dataCnt);
    config->maxMemory = (long long *) malloc(sizeof(long long) * dataCnt);
    config->maxStack = (int *) malloc(sizeof(int) * dataCnt);
    cJSON *temp;
    
    int i;
    if (config->judgeMode != ONLY_COMPILE_MODE) {
        for (i = 0; i < dataCnt; i++) {
            temp = cJSON_GetArrayItem(dataJson, i);
            char *inputData, *outputData, *stdAnswer;
            int maxCPUTime, maxStack;
            long long maxMemory;

            cJSON* atr;
            if ((atr = cJSON_GetObjectItem(temp, "inputData")) == NULL || !cJSON_IsString(atr)) {
                logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "Necessary attribute not is found or invalid : data");
                return 1;
            }
            inputData = (char * )malloc((strlen(atr->valuestring)+1) * sizeof(char));
            strcpy(inputData, atr->valuestring);
            
            
            if ((atr = cJSON_GetObjectItem(temp, "outputData")) == NULL || !cJSON_IsString(atr)) {
                logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "Necessary attribute not is found or invalid : data");
                return 1;
            }
            outputData = (char * )malloc((strlen(atr->valuestring)+1) * sizeof(char));
            strcpy(outputData, atr->valuestring);
            
            
            if ((atr = cJSON_GetObjectItem(temp, "stdAnswer")) == NULL || !cJSON_IsString(atr)) {   
                logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "Necessary attribute not is found or invalid : data");
                return 1;
            }
            stdAnswer = (char * )malloc((strlen(atr->valuestring)+1) * sizeof(char));
            strcpy(stdAnswer, atr->valuestring);
            

            if ((atr = cJSON_GetObjectItem(temp, "maxMemory")) == NULL || !cJSON_IsNumber(atr)) {
                logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "Necessary attribute not is found or invalid : data");
                return 1;
            }
            maxMemory = atr->valueint;
            if (maxMemory < 1){
                logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "Necessary attribute not is found or invalid : data");
                return 1;
            }

            if ((atr = cJSON_GetObjectItem(temp, "maxCPUTime")) == NULL || !cJSON_IsNumber(atr)) {
                logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "Necessary attribute not is found or invalid : data");
                return 1;
            }
            maxCPUTime = atr->valueint;
            if (maxCPUTime < 1){
                logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "Necessary attribute not is found or invalid : data");
                return 1;
            }
            if ((atr = cJSON_GetObjectItem(temp, "maxStack")) == NULL || !cJSON_IsNumber(atr)) {
                logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "Necessary attribute not is found or invalid : data");
                return 1;
            }
            maxStack = atr->valueint;
            if (maxStack < 1){
                logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "Necessary attribute not is found or invalid : data");
                return 1;
            }
            config->inputData[i] = (char * )malloc((strlen(inputData)+1) * sizeof(char));
            strcpy(config->inputData[i], inputData);
            
            config->outputData[i] = (char * )malloc((strlen(outputData)+1) * sizeof(char));
            strcpy(config->outputData[i], outputData);

            config->stdAnswer[i]  = (char * )malloc((strlen(stdAnswer)+1) * sizeof(char));
            strcpy(config->stdAnswer[i], stdAnswer);

            config->maxCPUTime[i] = maxCPUTime;
            config->maxMemory[i] = maxMemory;
            config->maxStack[i] = maxStack;
        }
    }
    if (config->translator.mode == COMPILER_MODE || config->translator.mode == DO_NOT_TANSLATE_MODE) {
        config->translator.interpreterInfoPath = (char * )malloc(sizeof(DEFAULT_INTERPRETER_INFO_PATH));
        strcpy(config->translator.interpreterInfoPath, DEFAULT_INTERPRETER_INFO_PATH);
        
        config->translator.interpreterPath = (char *) malloc((strlen(config->translator.compilerProductName) + 1) * sizeof(char));
        strcpy(config->translator.interpreterPath, config->translator.compilerProductName);
        config->translator.interpreterOptions = (char **) malloc(sizeof(char*));
        config->translator.interpreterOptions[0] = NULL;

    }
    config->caseNumber = dataCnt;
#ifdef __DEBUG
    __debugPrintJudgeConfig(config);
#endif
    return 0;
}
int createResultJson(struct timeval startTime, const JudgeConfig *config, const RunConfig *result, cJSON *resultJson) {
    struct timeval exitTime;
    gettimeofday(&exitTime, NULL);
    long realJudgeTime = (exitTime.tv_sec * 1000 + exitTime.tv_usec / 1000 - startTime.tv_sec * 1000 - startTime.tv_usec / 1000);
    long totalTime = 0;
    int dataCnt = config->caseNumber;
    int i;
    for (i = 0; i < dataCnt; i++) {
        totalTime += result[i].useRealTime;
    }
    
    cJSON * taskIDjson = cJSON_CreateNumber(result[0].taskID);
    cJSON_AddItemToObject(resultJson, "taskID", taskIDjson);

    char datetime[128];
    time_t nowTime;
    nowTime = time(NULL);
    strftime(datetime, MAX_LOG_LENGTH-1, "%Y-%m-%d %H:%M:%S", localtime(&nowTime));
    cJSON * doneTimeJson = cJSON_CreateString(datetime);
    cJSON_AddItemToObject(resultJson, "doneTime", doneTimeJson);
    
    cJSON * judgeTimeJson = cJSON_CreateNumber(realJudgeTime);
    cJSON_AddItemToObject(resultJson, "judgeTime", judgeTimeJson);

    cJSON * extraTimeJson = cJSON_CreateNumber(realJudgeTime - totalTime);
    cJSON_AddItemToObject(resultJson, "extraTime", extraTimeJson);

    cJSON* dataArrJson = cJSON_CreateArray();
    cJSON_AddItemToObject(resultJson, "result", dataArrJson);
    int curData;
    for (curData = 0; curData < dataCnt; curData++) {
        cJSON* eixtSignal = cJSON_CreateNumber(result[curData].exitSignal);
        cJSON* exitCode = cJSON_CreateNumber(result[curData].exitCode);
        cJSON* judgeResult = cJSON_CreateNumber(result[curData].result);
        cJSON* useCPUTime = cJSON_CreateNumber(result[curData].useCPUTime);
        cJSON* useMemory = cJSON_CreateNumber(result[curData].useMemory);
        cJSON* useRealTime = cJSON_CreateNumber(result[curData].useRealTime);
        cJSON* detail = cJSON_CreateString(result[curData].resultDetail);

        cJSON* dataJson =cJSON_CreateObject();
        cJSON_AddItemToObject(dataJson, "time", useCPUTime);
        cJSON_AddItemToObject(dataJson, "realTime", useRealTime);
        cJSON_AddItemToObject(dataJson, "memory", useMemory);
        cJSON_AddItemToObject(dataJson, "signal", eixtSignal);
        cJSON_AddItemToObject(dataJson, "code", exitCode);
        cJSON_AddItemToObject(dataJson, "result", judgeResult);
        cJSON_AddItemToObject(dataJson, "detail", detail);

        cJSON_AddItemToArray(dataArrJson, dataJson);
    }
#ifdef __DEBUG
    char buffer[MAX_DEBUG_INFO_LENGTH];
    sprintf(buffer, "Judge success : judger real time: %ldms, judger extra time : %ldms ", realJudgeTime, realJudgeTime - totalTime);
    logDebugInfoWithMessage(config->logPath, buffer);
#endif
    return 0;
}