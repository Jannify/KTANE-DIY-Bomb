#include "modules/base.h"

bool bombStarted = false;
byte minutes = 0;
byte seconds = 0;  //TODO: Implement timer
bool lastTry = false;
Timer<>::Task lastTryTask = nullptr;

void baseModuleInit(byte min, byte sec)
{
    minutes = min;
    seconds = sec;
    bombStarted = true;
}

bool toggleLastTry(void *) {
  digitalWrite(OUTPUT_Tries_1, !digitalRead(OUTPUT_Tries_1));
  digitalWrite(OUTPUT_Tries_2, !digitalRead(OUTPUT_Tries_2));
  return true;
}