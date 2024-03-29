#include "judger_config.h"
#include "boxlib.h"
#include "securitylib.h"
#include "loglib.h"



void run(JudgeConfig *config, int curDataNum) {


    struct rlimit maxStackLimit;
    maxStackLimit.rlim_cur = maxStackLimit.rlim_max = (rlim_t) config->maxStack[curDataNum];
    if (setrlimit(RLIMIT_STACK, &maxStackLimit) != 0) {
        logSystemErrorWithTaskID(config->logPath, config->taskID, BOX_SET_LIMIT_FAILED, "Can't set limit");
        raise(SIGUSR2);
        exit(EXIT_FAILURE);
    }

    struct rlimit maxCPUTimeLimit;
    maxCPUTimeLimit.rlim_cur = maxCPUTimeLimit.rlim_max = (rlim_t) (config->maxCPUTime[curDataNum] + 500 / 1000 + 1);
    if (setrlimit(RLIMIT_CPU, &maxCPUTimeLimit) != 0) {
        logSystemErrorWithTaskID(config->logPath, config->taskID, BOX_SET_LIMIT_FAILED, "Can't set limit");
        raise(SIGUSR2);
        exit(EXIT_FAILURE);
    }

    struct rlimit maxMemoryLimit;

    maxMemoryLimit.rlim_cur = maxMemoryLimit.rlim_max = (rlim_t) config->maxMemory[curDataNum] * 1024 * 4;

    if (setrlimit(RLIMIT_AS, &maxMemoryLimit) != 0) {

        logSystemErrorWithTaskID(config->logPath, config->taskID, BOX_SET_LIMIT_FAILED, "Can't set limit");
        raise(SIGUSR2);
        exit(EXIT_FAILURE);
    }

    struct rlimit maxOutputLimit;
    maxOutputLimit.rlim_cur = maxOutputLimit.rlim_max = (rlim_t) config->maxCharBuffer*8;
    if (setrlimit(RLIMIT_FSIZE, &maxOutputLimit) != 0) {
        logSystemErrorWithTaskID(config->logPath, config->taskID, BOX_SET_LIMIT_FAILED, "Can't set limit");
        raise(SIGUSR2);
        exit(EXIT_FAILURE);
    }

    struct rlimit maxProcessNumLimit;
    maxProcessNumLimit.rlim_cur = maxProcessNumLimit.rlim_max = config->translator->securityProcessNum;
    if (setrlimit(RLIMIT_NPROC, &maxProcessNumLimit) != 0) {
        logSystemErrorWithTaskID(config->logPath, config->taskID, BOX_SET_LIMIT_FAILED, "Can't set limit");
        raise(SIGUSR2);
        exit(EXIT_FAILURE);
    }

    if (config->iOMode == STD_IO) {

        if (freopen(config->inputData[curDataNum], "r", stdin) == NULL
            ||freopen(config->outputData[curDataNum], "w", stdout) == NULL
            ||freopen(config->translator->interpreterInfoPath, "w", stderr) == NULL
        )
        {

            logSystemErrorWithTaskID(config->logPath, config->taskID, BOX_DATA_REDIRECT_FAILED, "Can't redirect data");
            raise(SIGUSR2);
            exit(EXIT_FAILURE);
        }

    }

    gid_t group_list[] = {config->gid};
    if ((setgid(config->gid) == -1 || setgroups(sizeof(group_list) / sizeof(gid_t), group_list) == -1)) {
        logSystemErrorWithTaskID(config->logPath, config->taskID, BOX_SET_UID_FATLED, "Can't set uid/gid");
        raise(SIGUSR2);
        exit(EXIT_FAILURE);
    }
    if ( setuid(config->uid) == -1) {
        logSystemErrorWithTaskID(config->logPath, config->taskID, BOX_SET_UID_FATLED, "Can't set uid/gid");
        raise(SIGUSR2);
        exit(EXIT_FAILURE);
    }


    if (loadSeccompRules(config) != 0) {
        logSystemErrorWithTaskID(config->logPath, config->taskID, BOX_SECURITY_CONFIG_LOAD_FAILED, "Can't load security conifg");
        raise(SIGUSR2);
        exit(EXIT_FAILURE);
    }

   // freeJudgeConfig(config);
    char **envp = {NULL};
    execve(config->translator->interpreterPath, config->translator->interpreterOptions, envp);

    logSystemErrorWithTaskID(config->logPath, config->taskID, BOX_EXE_RUN_FAILED, "Can't run pending exe or interpreter");
    raise(SIGUSR2);
    exit(EXIT_FAILURE);
}
void runSpj(JudgeConfig *config, int curDataNum) {
    struct rlimit maxStackLimit;
    maxStackLimit.rlim_cur = maxStackLimit.rlim_max = (rlim_t)MAX_STACK_LIMIT;
    if (setrlimit(RLIMIT_STACK, &maxStackLimit) != 0) {
        logSystemErrorWithTaskID(config->logPath, config->taskID, BOX_SET_LIMIT_FAILED, "Can't set limit (spj)");
        raise(SIGUSR2);
        exit(EXIT_FAILURE);
    }

    struct rlimit maxCPUTimeLimit;
    maxCPUTimeLimit.rlim_cur = maxCPUTimeLimit.rlim_max = (rlim_t) (config->maxSPJTime + 500 / 1000 + 1);
    if (setrlimit(RLIMIT_CPU, &maxCPUTimeLimit) != 0) {
        logSystemErrorWithTaskID(config->logPath, config->taskID, BOX_SET_LIMIT_FAILED, "Can't set limit");
        raise(SIGUSR2);
        exit(EXIT_FAILURE);
    }

    struct rlimit maxMemoryLimit;
    maxMemoryLimit.rlim_cur = maxMemoryLimit.rlim_max = (rlim_t) config->maxSPJMemory * 1024 * 4;
    if (setrlimit(RLIMIT_AS, &maxMemoryLimit) != 0) {
        logSystemErrorWithTaskID(config->logPath, config->taskID, BOX_SET_LIMIT_FAILED, "Can't set limit");
        raise(SIGUSR2);
        exit(EXIT_FAILURE);
    }

    gid_t group_list[] = {config->gid};
    if (config->gid != -1 && (setgid(config->gid) == -1 || setgroups(sizeof(group_list) / sizeof(gid_t), group_list) == -1)) {
        logSystemErrorWithTaskID(config->logPath, config->taskID, BOX_SET_UID_FATLED, "Can't set gid/uid (spj)");
        raise(SIGUSR2);
        exit(EXIT_FAILURE);
    }
    if (config->uid != -1 && setuid(config->uid) == -1) {
        logSystemErrorWithTaskID(config->logPath, config->taskID, BOX_SET_UID_FATLED, "Can't set gid/uid (spj)");
        raise(SIGUSR2);
        exit(EXIT_FAILURE);
    }
    if (loadSeccompRulesForSPJ(config) != 0) {
        logSystemErrorWithTaskID(config->logPath, config->taskID, BOX_SECURITY_CONFIG_LOAD_FAILED, "Can't load security config (spj)");
        raise(SIGUSR2);
        exit(EXIT_FAILURE);
    }
    
    if (config->iOMode == STD_IO) {
        freeJudgeConfig(config);
        execl(config->sPJPath, config->sPJName, config->inputData[curDataNum], config->stdAnswer[curDataNum], config->outputData[curDataNum], NULL);
    } else {
        freeJudgeConfig(config);
        execl(config->sPJPath, config->sPJName, config->inputData[curDataNum], config->stdAnswer[curDataNum], FILEIO_OUTPUT_PATH, NULL);
    }
    
    logSystemErrorWithTaskID(config->logPath, config->taskID, BOX_EXE_RUN_FAILED, "Can't run spj");
    raise(SIGUSR2);
    
    exit(EXIT_FAILURE);
}



