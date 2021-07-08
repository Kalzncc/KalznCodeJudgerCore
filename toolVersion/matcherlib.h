#ifndef MATCHERLIB_H
#define MATCHERLIB_H

#include "judgerlib.h"



void matchResult(const JudgerConfig *judgerConfig, const JudgeConfig * config, int curCase, RunConfig * result);
void matchWithSPJ(const JudgerConfig * judgerConfig, const JudgeConfig * config, int curCase, RunConfig * result, const char * logPath);

#endif