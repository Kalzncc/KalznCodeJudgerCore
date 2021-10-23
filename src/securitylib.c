#include "judger_config.h"
#include "securitylib.h"


int loadSeccompRules(const JudgeConfig *config) {
        
   int syscalls_blacklist[] = {
                                SCMP_SYS(fork), SCMP_SYS(vfork),
                                SCMP_SYS(kill), SCMP_SYS(fchdir),
#ifdef __NR_execveat
                                SCMP_SYS(execveat)
#endif
                               };
    int syscalls_blacklist_length = sizeof(syscalls_blacklist) / sizeof(int);
    scmp_filter_ctx ctx = NULL;
    /* load seccomp rules */
    ctx = seccomp_init(SCMP_ACT_ALLOW);
    if (!ctx) {
        return -1;
    }
    int i;
    for (i = 0; i < syscalls_blacklist_length; i++) {
        if (seccomp_rule_add(ctx, SCMP_ACT_KILL, syscalls_blacklist[i], 0) != 0) {
            return -1;
        }
    }
    if (seccomp_rule_add(ctx, SCMP_ACT_ERRNO(EACCES), SCMP_SYS(socket), 0) != 0) {
        return -1;
    }
    if (seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(execve), 1, SCMP_A0(SCMP_CMP_NE, (scmp_datum_t)(config->translator.interpreterPath))) != 0) {
        return -1;
    }
    
    if (seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(open), 1, SCMP_CMP(1, SCMP_CMP_MASKED_EQ, O_WRONLY, O_WRONLY)) != 0) {
        return -1;
    }
    if (seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(open), 1, SCMP_CMP(1, SCMP_CMP_MASKED_EQ, O_RDWR, O_RDWR)) != 0) {
        return -1;
    }
    
    if (seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(openat), 1, SCMP_CMP(2, SCMP_CMP_MASKED_EQ, O_WRONLY, O_WRONLY)) != 0) {
        return -1;
    }
    if (seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(openat), 1, SCMP_CMP(2, SCMP_CMP_MASKED_EQ, O_RDWR, O_RDWR)) != 0) {
        return -1;
    }

    if (seccomp_load(ctx) != 0) {
        return -1;
    }
    seccomp_release(ctx);
    return 0;
}

int loadSeccompRulesForSPJ(const JudgeConfig *config) {
    return loadSeccompRules(config);
}
