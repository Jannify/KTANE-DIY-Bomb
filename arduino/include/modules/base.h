#pragma once

#include "common.h"

extern bool bombStarted;
extern Timer<>::Task lastTryTask;

void baseModuleInit(unsigned short);
void baseModuleLogicLoop();
void setTries(byte);
bool toggleLastTry(void *);