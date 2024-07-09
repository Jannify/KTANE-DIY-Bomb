#pragma once

#include "common.h"

extern Timer<>::Task lastTryTask;
extern unsigned short lastSecondsLeft;

void baseModuleInit(unsigned short);
void baseModuleLogicLoop();
void setTries(byte);
bool toggleLastTry(void *);
void basePowerOff();