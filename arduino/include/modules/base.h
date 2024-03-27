#pragma once

#include "common.h"

extern bool bombStarted;
extern Timer<>::Task lastTryTask;

void baseModuleInit(byte, byte);
bool toggleLastTry(void *);