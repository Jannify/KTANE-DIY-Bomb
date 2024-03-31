#pragma once

#include "common.h"

extern bool bombStarted;
extern Timer<>::Task lastTryTask;

void baseModuleInit(unsigned short);
void baseModuleLogicLoop();
bool toggleLastTry(void *);