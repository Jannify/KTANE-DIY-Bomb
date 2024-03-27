#pragma once

#include "common.h"

extern Timer<>::Task lastTryTask;

void baseModuleInit(byte, byte);
bool toggleLastTry(void *);