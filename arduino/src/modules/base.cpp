#include "modules/base.h"

#include <TM1637Display.h>

TM1637Display clock(OUTPUT_ClockDisplay_Clk, OUTPUT_ClockDisplay_Data);
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
    memoryTriesBuffer |= 0b01100000;
    lastTryTask = timer.every(600, toggleLastTry);
  }
  else if (tries >= 1)
  {
    memoryTriesBuffer |= 0b01100000;
    memoryTriesBuffer &= 0b11011111;
  }
  else
  {
    memoryTriesBuffer &= 0b10011111;
  }

  shiftOutLED(OUTPUT_Clock_MemoryTriesIndicator, memoryTriesBuffer);
}

bool toggleLastTry(void *)
{
  memoryTriesBuffer ^= 0b01100000;
  return true;
}