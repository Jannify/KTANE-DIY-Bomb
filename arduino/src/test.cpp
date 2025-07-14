#include "common.h"
#include "modules/base.h"
#include "modules/frame.h"
#include "modules/big_button.h"
#include "modules/memory.h"
#include "modules/morse.h"
#include "modules/simon.h"
#include "modules/password.h"
#include "modules/wires.h"

uint8_t counter = 0;
uint8_t last_counter = 0;
unsigned long startTime = LONG_MAX;

void test_setup()
{
  startTime = millis();

  baseModuleInit(3000);
  startBomb();
  digitalWrite(OUTPUT_Morse_LED, HIGH);

  char test_indicator[] = "ABC";
  setIndicatorText(test_indicator);

  char test_serial[] = "123456";
  setSerialNumber(test_serial);

  setIndicatorLED(true);
  frameStart();

  morseInitRead(0);

  //passwordInit();
  //char test_password[] = "testi";
  //setPassword(test_password);
  //showPassword();

  int bigNumber = 1;
  int smallNumber1 = 2;
  int smallNumber2 = 3;
  int smallNumber3 = 1;
  int smallNumber4 = 2;
  shiftOut(OUTPUT_Register_Data_Memory, OUTPUT_Clock_Memory_Big, MSBFIRST, bigNumber);
  shiftOut(OUTPUT_Register_Data_Memory, OUTPUT_Clock_Memory_Left, MSBFIRST, smallNumber2 | (smallNumber1 << 4));
  shiftOut(OUTPUT_Register_Data_Memory, OUTPUT_Clock_Memory_Right, MSBFIRST, smallNumber4 | (smallNumber3 << 4));
  //memoryInit();
  //memoryPowerOff();
}

void test_loop()
{
  counter = ((millis() - startTime) / 1000) % 16;

  baseModuleLogicLoop();

  if (last_counter != counter)
  {
    last_counter = counter;

    shiftOutLED(OUTPUT_Clock_SimonSays, 1 << (counter % 4));

    morseFrequencyIndex = counter;
    setDisplayText();

    setSolvedModules(1 << (counter % 7), 0);

    byte numb = counter % 4;
    byte num1 = (counter + 1) % 4;
    byte num2 = (counter + 2) % 4;
    byte num3 = (counter + 3) % 4;
    byte num4 = (counter + 4) % 4;
    byte data0 = numb << 6;
    data0 = data0 | (num1 << 4);
    data0 = data0 | (num2 << 2);
    data0 = data0 | num3;
    byte data1 = num4 << 6;
    //memorySetNumber(0xFF, 0x00);
    //memoryStart();
  }

  if (!loopLogicButtonCooldown)
  {
    //morseLogicButtonLoop();
  }

  if (!loopSerialWriteCooldown)
  {
    bigButtonSerialWriteLoop();
    wiresSerialWriteLoop();
    passwordSerialWriteLoop();
    simonSerialWriteLoop();
    memorySerialWriteLoop();
  }
}