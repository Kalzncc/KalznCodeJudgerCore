#ifndef SECURITYLIB_H
#define SECURITYLIB_H
#include <seccomp.h>
#include "judger_config.h"
int loadSeccompRules(const JudgeConfig *config);
int loadSeccompRulesForSPJ(JudgeConfig *config);
#endif