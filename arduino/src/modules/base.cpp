#include "modules/base.h"

#include <TM1637.h>

TM1637 clock(OUTPUT_ClockDisplay_Data, OUTPUT_ClockDisplay_Clk);
unsigned long timeAtStart = LONG_MAX;
unsigned short givenBombTimeSeconds = 0;

unsigned short lastSecondsLeft = USHRT_MAX;

bool lastTry = false;
Timer<>::Task lastTryTask = nullptr;

void baseModuleInit(unsigned short sec)
{
  givenBombTimeSeconds = sec;
  timeAtStart = millis();
  clock.setupDisplay(true, 7);
}

unsigned short getSecondsLeft()
{
  return givenBombTimeSeconds - ((millis() - timeAtStart) / 1000);
}

void baseModuleLogicLoop()
{
  unsigned short combinedSecondsLeft = getSecondsLeft();
  if (combinedSecondsLeft <= 3500 && combinedSecondsLeft != lastSecondsLeft)
  {
    lastSecondsLeft = combinedSecondsLeft;
    baseSetClockToSecondsLeft(combinedSecondsLeft);
    //tone(OUTPUT_BUZZER, 2000, 75);
  }

  if (combinedSecondsLeft >= (USHRT_MAX - 10))
  {
    sendSerialData(0xE);
  }
}

void baseSetClockToSecondsLeft(unsigned short combinedSecondsLeft) {
    unsigned short minutesLeft = combinedSecondsLeft / 60;
    unsigned short secondsLeft = combinedSecondsLeft - minutesLeft * 60;

    byte dot = _BV(4);
    if (secondsLeft % 2)
    {
      dot = _BV(3);
    }

    clock.setDisplayToDecNumber(minutesLeft * 100 + secondsLeft, dot);
}

void baseDisplayMsgOnClock(const char *msg, bool clear)
{
  clock.setupDisplay(true, 7);
  clock.clearDisplay();
  clock.setDisplayToString(msg);

  if(clear) {
    delay(1000);
    clock.setupDisplay(false, 7);
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
    memoryTriesBuffer |= 0b00000110;
    lastTryTask = timer.every(600, toggleLastTry);
  }
  else if (tries >= 1)
  {
    memoryTriesBuffer |= 0b00000110;
    memoryTriesBuffer &= 0b11111011;
  }
  else
  {
    memoryTriesBuffer &= 0b11111001;
  }

  shiftOutLED(OUTPUT_Clock_MemoryTriesIndicator, memoryTriesBuffer);
}

bool toggleLastTry(void *)
{
  memoryTriesBuffer ^= 0b00000110;
  shiftOutLED(OUTPUT_Clock_MemoryTriesIndicator, memoryTriesBuffer);
  return true;
}

void basePowerOff()
{
  setTries(0);
}