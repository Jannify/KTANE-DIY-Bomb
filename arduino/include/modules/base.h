#pragma once

#include "common.h"

extern Timer<>::Task lastTryTask;
extern unsigned short lastSecondsLeft;

void baseModuleInit(unsigned short);
void baseModuleLogicLoop();
void baseSetClockToSecondsLeft(unsigned short combinedSecondsLeft);
void baseDisplayMsgOnClock(const char *msg, bool clear = false);
void setTries(byte);
bool toggleLastTry(void *);
void basePowerOff();