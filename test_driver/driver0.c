/* 用于并发测试 */

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>
#include <grp.h>
#include <dlfcn.h>
#include <sched.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mount.h>
#include <sys/wait.h>
#include <stdarg.h>
#include <sys/file.h>
#include <sys/resource.h>
#include <errno.h>

#define PROCESS_NUMBER 10
#define MAXN 1024
char buffer[MAXN];
int main(int argc, char * argv[]) {
    
    if ( chdir("../sh") ) {
        sprintf(buffer, "Can not access sh.");
        fprintf(stderr, "%s\n", buffer);
        exit(EXIT_FAILURE);
    }
    int i;
    for (i = 0; i < PROCESS_NUMBER; i++) {
        pid_t judgerID = fork();
        if (judgerID < 0) {
            sprintf(buffer, "Error run judger in %d.", i);
            fprintf(stderr, "%s\n", buffer);
        } else if (judgerID == 0) {
            system("sh run.sh");
            sprintf(buffer, "Done in %d.", i);
            printf("%s\n", buffer);
            exit(0);
        }
    }
    wait(NULL);
    exit(0);
}