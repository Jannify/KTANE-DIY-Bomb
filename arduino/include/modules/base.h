#pragma once

#include "common.h"

extern Timer<>::Task lastTryTask;

void baseModuleInit(unsigned short);
void baseModuleLogicLoop();
unsigned short getSecondsLeft();
void setTries(byte);
bool toggleLastTry(void *);