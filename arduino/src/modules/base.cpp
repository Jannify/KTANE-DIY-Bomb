#include "modules/base.h"

#include <TM1637Display.h>

bool bombStarted = false;

TM1637Display clock(2, 3); // Currently not working as we need separate pins
uint8_t clock_data[4];
long timeAtStart = LONG_MAX;
unsigned short givenBombTimeSeconds = 0;

bool lastTry = false;
Timer<>::Task lastTryTask = nullptr;

void baseModuleInit(unsigned short sec)
{
  givenBombTimeSeconds = sec;
  timeAtStart = millis();
  bombStarted = true;
  clock.setBrightness(0x0f);
}

void baseModuleLogicLoop()
{
  unsigned short combinedSecondsLeft = givenBombTimeSeconds - ((millis() - timeAtStart) / 1000);
  if (combinedSecondsLeft >= 0)
  {
    unsigned short minutesLeft = combinedSecondsLeft / 60;
    unsigned short secondsLeft = combinedSecondsLeft - minutesLeft * 60;
    clock_data[0] = clock.encodeDigit(minutesLeft / 10);
    clock_data[1] = clock.encodeDigit(minutesLeft % 10);
    clock_data[2] = clock.encodeDigit(secondsLeft / 10);
    clock_data[3] = clock.encodeDigit(secondsLeft % 10);

    if (combinedSecondsLeft % 2)
    {
      clock_data[1] |= SEG_DP;
    }
    clock.setSegments(clock_data);
  }
}

void setTries(byte tries)
{
  if (lastTryTask != nullptr)
  {
    timer.cancel(lastTryTask);
    lastTryTask = nullptr;
  }
  if (tries >= 2)
  {
    digitalWrite(OUTPUT_Tries_1, HIGH);
    digitalWrite(OUTPUT_Tries_2, HIGH);
    lastTryTask = timer.every(600, toggleLastTry);
  }
  else if (tries >= 1)
  {
    digitalWrite(OUTPUT_Tries_1, HIGH);
    digitalWrite(OUTPUT_Tries_2, LOW);
  }
  else
  {
    digitalWrite(OUTPUT_Tries_1, LOW);
    digitalWrite(OUTPUT_Tries_2, LOW);
  }
}

bool toggleLastTry(void *)
{
  digitalWrite(OUTPUT_Tries_1, !digitalRead(OUTPUT_Tries_1));
  digitalWrite(OUTPUT_Tries_2, !digitalRead(OUTPUT_Tries_2));
  return true;
}