#include "compilerlib.h"
#include "judger_config.h"

int compileCode(const char * compilerPath,  char ** argv) {
    char *option;
    
    char cmdBuffer[MAX_CMD_STR_LENGTH];
    
    strcpy(cmdBuffer, compilerPath);
    int i;
    int skipFirst = 1;
    for (option = argv[0]; option != NULL; option++) {
        
        if (skipFirst) skipFirst = 0;
        else {
            strcat(cmdBuffer, " ");
            strcat(cmdBuffer, option);
        }
    }
    strcat(cmdBuffer, " 2>");
    strcat(cmdBuffer, COMPILER_INFO_PATH);
    system(cmdBuffer);
    return 0;
}
