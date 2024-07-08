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


void setSolvedModules(byte data0, byte data1)
{
  activeModules[0] = data0 & 0b00000001;
  activeModules[1] = data0 & 0b00000010;
  activeModules[2] = data0 & 0b00000100;
  activeModules[3] = data0 & 0b00001000;
  activeModules[4] = data0 & 0b00010000;
  activeModules[5] = data0 & 0b00100000;
  activeModules[6] = data0 & 0b01000000;

  // activeModules[7] = data1 & 0b00000001;
  // activeModules[8] = data1 & 0b00000010;
  // activeModules[9] = data1 & 0b00000100;
  // activeModules[10] = data1 & 0b00001000;
  // activeModules[11] = data1 & 0b00010000;

  shiftOutLED(OUTPUT_Clock_ModulesSolved_1, data0);
  //shiftOutLED(OUTPUT_Clock_ModulesSolved_2, data1);
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
