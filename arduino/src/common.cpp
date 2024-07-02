#include "common.h"

Timer<20> timer;

TCA9548 multiplexer(0x70);

bool activeModules[11] = {false};
bool bombStarted = false;

bool loopLogicButtonCooldown = false;
bool loopSerialWriteCooldown = false;

byte memoryTriesBuffer = 0x00;

void startBomb()
{
  for (size_t i = 0; i < 6; i++)
  {
    activeModules[i] = true;
  }

  for (size_t i = 6; i < 11; i++)
  {
    activeModules[i] = false;
  }

  bombStarted = true;
}

void engageLogicCooldown()
{
  loopLogicButtonCooldown = true;
  timer.in(50, [](void *) -> bool {
      loopLogicButtonCooldown = false;
      return false; 
    });
}

void engageSerialWriteCooldown()
{
  loopSerialWriteCooldown = true;
  timer.in(50, [](void *) -> bool {
      loopSerialWriteCooldown = false;
      return false; 
    });
}

// Bitfield of Modules solved (5 zeros + from 10 to 0)
void setSolvedModules(byte data0, byte data1)
{
  activeModules[10] = data0 & B00000100;
  activeModules[9] = data0 & B00000010;
  activeModules[8] = data0 & B00000001;

  activeModules[7] = data1 & B10000000;
  activeModules[6] = data1 & B01000000;
  activeModules[5] = data1 & B00100000;
  activeModules[4] = data1 & B00010000;
  activeModules[3] = data1 & B00001000;
  activeModules[2] = data1 & B00000100;
  activeModules[1] = data1 & B00000010;
  activeModules[0] = data1 & B00000001;
}

bool setPinLow(void *argument)
{
  digitalWrite((int)argument, LOW);
  return false;
}

void shiftOutLED(byte clockPin, byte val)
{
  for (byte i = 0; i < 8; i++)
  {
    digitalWrite(OUTPUT_Register_Data_LED, (val & 128) != 0);
    val <<= 1;

    digitalWrite(clockPin, HIGH);
    digitalWrite(clockPin, LOW);
  }
  digitalWrite(OUTPUT_Register_Data_LED, 0);
  delay(2);
}

void reset()
{
  // Reset BigButton
  // Reset 7Bit
  // Reset I2C
  // Reset SPI
  timer.cancel();
  bombStarted = false;
  // digitalWrite(OUTPUT_RESET, LOW); //TODO: Evaluate need
}
