#include "spjlib.h"
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

void _start_judge(int argc, char *argv[], FILE *&in, FILE *&sout, FILE *&out) {
    if (argc != 4) {
            
        exit(_ARGV_ERROR);
    }
    in = fopen(argv[1], "r");
    sout = fopen(argv[2], "r");
    out = fopen(argv[3], "r");
    if (in == NULL || sout == NULL || out == NULL) 
        exit(_CAN_NOT_OPEN_DATA_FILE);

}