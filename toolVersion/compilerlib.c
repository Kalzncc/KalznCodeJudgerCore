#include "compilerlib.h"
#include "judger_config.h"

int compileCode(const char * compilerPath,  char ** argv) {
    
    char cmdBuffer[MAX_CMD_STR_LENGTH];
    
    strcpy(cmdBuffer, compilerPath);
    int i;
    int skipFirst = 1;
    for (i = 0; argv[i] != NULL; i++) {
        
        if (skipFirst) skipFirst = 0;
        else {
            strcat(cmdBuffer, " ");
            strcat(cmdBuffer, argv[i]);
        }
    }
    
    strcat(cmdBuffer, " 2>");
    strcat(cmdBuffer, COMPILER_INFO_PATH);
#ifdef __DEBUG
    printf("compiler cmd : %s\n", cmdBuffer);
#endif
    system(cmdBuffer);
    return 0;
}
