#include "judger_config.h"
#include "cjson/cJSON.h"
#include "cjson/cJSON_Utils.h"
#include "loglib.h"
#include "judgerlib.h"
#include "jsonlib.h"



char jsonStr[MAX_JSON_CHAR_NUM+2];


int main(int argc, char *argv[]) {
    
#ifdef __DEBUG
    FILE *makeFile = fopen("../test_sample/setting.info", "r");
    fscanf(makeFile, "%d", &argc);
    char buf[128];
    strcpy(buf, argv[0]);
    printf("%s\n", buf);
    argv = (char **)malloc(sizeof(char*) * (argc+1));
    argv[0] = (char*) malloc(sizeof(char) * (strlen(buf)+1));
    strcpy(argv[0], buf);
    for (int i = 1; i <= argc; i++) {
        fscanf(makeFile, "%s", buf);
        argv[i] = (char*) malloc(sizeof(char) * (strlen(buf)+1));
        strcpy(argv[i], buf);
    }
    argc++;
    printf("%d\n", argc);
    for (int i = 0; i < argc; i++) {
        printf("%s\n", argv[i]);
    }
#endif
    
    struct timeval startTime, exitTime;
    gettimeofday(&startTime, NULL);
    char *logPath = (char *) malloc(sizeof(char) * (strlen(DEFAULT_LOG_PATH)+1));
    strcpy(logPath, DEFAULT_LOG_PATH);
    FILE * jsonFile;
    if (argc != 2 && argc != 3) {
        logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "Input judge config invalid.");
        goto FAIL_EXIT0;
    } 
    if (argc == 3) {
        logPath = (char *) malloc(sizeof(char) * (strlen(argv[2])+1));
        strcpy(logPath, argv[2]);
    }
    if ( (jsonFile = fopen(argv[1], "r") ) == NULL) {
        logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "Can't open the json file.");
        goto FAIL_EXIT0;
    }
    char ch; int cnt = 0;
    
    while( (ch = fgetc(jsonFile))!=EOF && cnt <= MAX_JSON_CHAR_NUM)  {
        jsonStr[cnt++] = ch;
    }
    if (cnt > MAX_JSON_CHAR_NUM) {
        logSystemErrorWithMessage(logPath,INVALID_JUDGE_CONFIG, "Json file too large.");
        goto FAIL_EXIT0;
    }
    jsonStr[cnt] = '\0';
    JudgeConfig* config = (JudgeConfig *)malloc(sizeof(JudgeConfig));
    config->translator = (TranslatorConfig *) malloc(sizeof(TranslatorConfig));

    if (loadJudgeConfig(jsonStr, config, logPath))  goto FAIL_EXIT1;
    

#ifdef __DEBUG
    logDebugInfoWithMessage(config->logPath, "Load json success.");
#endif
    
    
    RunConfig * result = judge(config);

    cJSON *resultJson = cJSON_CreateObject();
    createResultJson(startTime, config, result, resultJson);

    FILE *resultFile = NULL;
    if ((resultFile = fopen(config->resultPath, "w")) == NULL) {
        logSystemErrorWithTaskID(config->logPath, config->taskID, WRITE_RESULT_FILE_FAILED, "Can't open result file");
        goto FAIL_EXIT2;
    }


#ifdef __DEBUG
    fprintf(resultFile, "%s", cJSON_Print(resultJson));
#else 
    fprintf(resultFile, "%s", cJSON_PrintUnformatted(resultJson));
#endif
    fclose(resultFile);

    free(config);
    free(result);

    return 0;

    
FAIL_EXIT2:
    fclose(resultFile);
FAIL_EXIT1:
    
FAIL_EXIT0:
    exit(EXIT_FAILURE);
}