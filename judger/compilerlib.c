#include "compilerlib.h"
#include "judger_config.h"

int compileCode(const char * compilerPath,  const char * compilerInfoPath, char ** argv) {
    
    char cmdBuffer[MAX_CMD_STR_LENGTH+2];
    int length = strlen(compilerPath);
    if (length > MAX_CMD_STR_LENGTH) {
        return 1;
    }
    strcpy(cmdBuffer, compilerPath);
    int i;
    int skipFirst = 1;
    for (i = 0; argv[i] != NULL; i++) {
        if (skipFirst) skipFirst = 0;
        else {
            int argvLength;
            if (length + 1 + (argvLength = strlen(argv[i])) > MAX_CMD_STR_LENGTH) {
                return 1;
            }
            strcat(cmdBuffer, " ");
            strcat(cmdBuffer, argv[i]);
            length += 1 + argvLength;
        }
    }
    if (length + 3 + strlen(compilerInfoPath) > MAX_CMD_STR_LENGTH) {
        return 1;
    }
    strcat(cmdBuffer, " 2>");
    strcat(cmdBuffer, compilerInfoPath);

    system(cmdBuffer);
    return 0;
}
