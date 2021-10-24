#include "judger_config.h"

void freeJudgeConfig(JudgeConfig* config) {
    free(config->workSpacePath);
    free(config->inputData);
    free(config->outputData);
    free(config->stdAnswer);
    free(config->maxCPUTime);
    free(config->maxMemory);
    free(config->maxStack);
    free(config->logPath);
    free(config->sPJName);
    free(config->sPJPath);
    free(config->translator->compilerPath);
    free(config->translator->compilerOptions);
    free(config->translator->compilerProductName);
    free(config->translator->interpreterPath);
    free(config->translator->interpreterOptions);

}
void freeRunConfig(RunConfig *config) {
    free(config->resultDetail);
}

#ifdef __DEBUG
#include <stdio.h>
void __debugPrintnum(const char * str, long long value) {
    fprintf(stderr, "%s : %lld\n", str, value);
}
void __debugPrints(const char * str, const char* value) {
    fprintf(stderr, "%s : %s\n", str, value);
}
void __debugPrintJudgeConfig(JudgeConfig *judgeConfig) {
    __debugPrintnum("maxCharBuffer", judgeConfig->maxCharBuffer);
    __debugPrintnum("maxSPJTime", judgeConfig->maxSPJTime);
    __debugPrintnum("maxSPJMemory", judgeConfig->maxSPJMemory);
    __debugPrintnum("taskID", judgeConfig->taskID);
    __debugPrintnum("uid", judgeConfig->uid);
    __debugPrintnum("gid", judgeConfig->gid);
    __debugPrintnum("judgeMode", judgeConfig->judgeMode);
    __debugPrintnum("iOMode", judgeConfig->iOMode);
    __debugPrintnum("strictMode", judgeConfig->strictMode);
    __debugPrints("workSpacePath", judgeConfig->workSpacePath);
    __debugPrints("logPath", judgeConfig->logPath);
    __debugPrintnum("isSPJ", judgeConfig->isSPJ);
    if (judgeConfig->isSPJ) {
        __debugPrints("spjExePath", judgeConfig->sPJPath);
        __debugPrints("spjExeName", judgeConfig->sPJName);
    }
    __debugPrintnum("translator-mode", judgeConfig->translator->mode);
    if (judgeConfig->translator->mode != INTERPRETER_MODE) {
        __debugPrints("compilerPath", judgeConfig->translator->compilerPath);
        int i;
        fprintf(stderr, "compilerOption : ");
        for (i = 0; judgeConfig->translator->compilerOptions[i] != NULL; i++) {
            fprintf(stderr, "%s ", judgeConfig->translator->compilerOptions[i]);
        }
        fputs("", stderr);
        __debugPrints("compilerProductName", judgeConfig->translator->compilerProductName);
    }
    if (judgeConfig->translator->mode != COMPILER_MODE) {
        __debugPrints("interpreterPath", judgeConfig->translator->interpreterPath);
        int i;
        fprintf(stderr, "interpreterOptions : ");
        for (i = 0; judgeConfig->translator->interpreterOptions[i] != NULL; i++) {
            fprintf(stderr, "%s ", judgeConfig->translator->interpreterOptions[i]);
        }
        fputs("", stderr);
    }
    int i;
    for (i = 0; i < judgeConfig->caseNumber; i++) {
        fprintf(stderr, "data %d ------- : \n", i);
        __debugPrints("inputData", judgeConfig->inputData[i]);
        __debugPrints("outputData", judgeConfig->outputData[i]);
        __debugPrints("stdAnswer", judgeConfig->stdAnswer[i]);
        __debugPrintnum("maxCPUTime", judgeConfig->maxCPUTime[i]);
        __debugPrintnum("maxMemory", judgeConfig->maxMemory[i]);
        __debugPrintnum("maxStack", judgeConfig->maxStack[i]);
    }
}
void __debugPrintRunConfig(RunConfig *config) {
    
}
#endif
