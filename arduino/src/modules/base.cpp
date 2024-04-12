#include "modules/base.h"

#include <TM1637Display.h>

TM1637Display clock(OUTPUT_Clock_Clk, OUTPUT_Clock_Data);
uint8_t clock_data[4];
unsigned long timeAtStart = LONG_MAX;
unsigned short givenBombTimeSeconds = 0;

unsigned short lastSecondsLeft = USHRT_MAX;

bool lastTry = false;
Timer<>::Task lastTryTask = nullptr;

void baseModuleInit(unsigned short sec)
{
  givenBombTimeSeconds = sec;
  timeAtStart = millis();
  clock.setBrightness(0x0f);
}

void baseModuleLogicLoop()
{
  unsigned short combinedSecondsLeft = getSecondsLeft();
  if (combinedSecondsLeft <= 3500 && combinedSecondsLeft != lastSecondsLeft)
  {
    lastSecondsLeft = combinedSecondsLeft;
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
    tone(OUTPUT_BUZZER, 2000, 75);
  }

  if (combinedSecondsLeft >= (USHRT_MAX - 10))
  {
    Serial.write(0xF);
    engageLogicCooldown();
    reset();
  }
}

unsigned short getSecondsLeft()
{
  return givenBombTimeSeconds - ((millis() - timeAtStart) / 1000);
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