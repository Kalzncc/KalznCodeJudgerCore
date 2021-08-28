#ifndef MATCHERLIB_H
#define MATCHERLIB_H

#include "judger_config.h"



void matchResult(const JudgeConfig * config, int curCase, RunConfig * result);
void matchWithSPJ(const JudgeConfig * config, int curCase, RunConfig * result);

#endif