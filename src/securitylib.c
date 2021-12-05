#include "judger_config.h"
#include "securitylib.h"


int loadSeccompRules(const JudgeConfig *config) {

       int syscalls_blacklist[] = {
                                SCMP_SYS(kill), /* can not kill process */
                                SCMP_SYS(msgsnd), /* can not send message */

                                /*add clone() in blacklist will cause jmv can not initialize*/
                                /*please use the 'security process number' parameter in JudgerConfig limit process number*/
                                SCMP_SYS(fork),
                                SCMP_SYS(vfork),

                                SCMP_SYS(socket), /* can not create socket  */
                                SCMP_SYS(connect), /* can not connect remote host */
                                SCMP_SYS(accept), /* can not accept socket request */
                                SCMP_SYS(send), /* can not send from socket */
                                SCMP_SYS(sendto), /* can not send from UDP*/
                                SCMP_SYS(listen), /*can not listen port*/

                                SCMP_SYS(setpriority), /* can not set process priority */
                                SCMP_SYS(modify_ldt), /*can not set ldt*/
                                SCMP_SYS(sched_setparam), /*can not set process param*/

                                SCMP_SYS(chdir), /*can not change work dir*/
                                SCMP_SYS(mkdir), /*can not make dir*/
                                SCMP_SYS(mknod), /*can not make node*/
                                SCMP_SYS(umount), /*can not umount file system*/
                                SCMP_SYS(setrlimit), /*can not set resource limit*/
                                SCMP_SYS(adjtimex), /*can not set system clock*/
                                SCMP_SYS(stime), /*can not set time*/
                                SCMP_SYS(setuid), /*can not set uid*/
                                SCMP_SYS(setgid) /*can not set gid*/

                               };
    int syscalls_blacklist_length = sizeof(syscalls_blacklist) / sizeof(int);
    scmp_filter_ctx ctx = NULL;


    ctx = seccomp_init(SCMP_ACT_ALLOW);
    if (!ctx) {
        return -1;
    }
    for (int i = 0; i < syscalls_blacklist_length; i++) {
        if (seccomp_rule_add(ctx, SCMP_ACT_KILL, syscalls_blacklist[i], 0) != 0) {
            return -1;
        }
    }
    /* process can not execute file except the pending program*/
    if (seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(execve), 1, SCMP_A0(SCMP_CMP_NE, (scmp_datum_t)(config->translator->interpreterPath))) != 0) {
        return -1;
    }

    if (config->iOMode == FILE_IO) { /*if IO mode is file io, process only is allowed to open default file_IO_mode files.*/
        if (seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(open), 1, SCMP_A0(SCMP_CMP_EQ, (scmp_datum_t)(FILEIO_INPUT_PATH))) != 0) {
            return -1;
        }
        if (seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(open), 1, SCMP_A0(SCMP_CMP_EQ, (scmp_datum_t)(FILEIO_OUTPUT_PATH))) != 0) {
            return -1;
        }
        if (seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(openat), 1, SCMP_A1(SCMP_CMP_EQ, (scmp_datum_t)(FILEIO_INPUT_PATH))) != 0) {
            return -1;
        }
        if (seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(openat), 1, SCMP_A1(SCMP_CMP_EQ, (scmp_datum_t)(FILEIO_OUTPUT_PATH))) != 0) {
            return -1;
        }

    } else { /*if IO mode is std io, process is not allowed to open any file.*/
//        if (seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(open), 0 ) != 0) {
//            return -1;
//        }
//        if (seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(openat), 0 ) != 0) {
//            return -1;
//        }
    }

    if (seccomp_load(ctx) != 0) {
        return -1;
    }
    seccomp_release(ctx);
    return 0;
}

int loadSeccompRulesForSPJ(JudgeConfig *config) {
    int c_mode = config->iOMode;
    config->iOMode = STD_IO;
    int res = loadSeccompRules(config);
    config->iOMode = c_mode;
    return res;
}
