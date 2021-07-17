#include "judger_config.h"

void freeJudgerConfig(JudgerConfig config) {
    return;
}
void freeJudgeConfig(JudgeConfig config) {
    
    free(config.workSpacePath);
    free(config.inputData);
    free(config.outputData);
    free(config.stdAnswer);
    free(config.maxCPUTime);
    free(config.maxMemory);
    free(config.maxStack);
    free(config.logPath);
    free(config.sPJName);
    free(config.sPJPath);
    free(config.translator.compilerPath);
    free(config.translator.compilerOptions);
    free(config.translator.compilerProductName);
    free(config.translator.interpreterPath);
    free(config.translator.interpreterOptions);

}
void freeRunConfig(RunConfig config) {
    free(config.resultDetail);
}