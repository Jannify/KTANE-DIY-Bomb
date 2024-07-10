#include "common.h"
#include "modules/base.h"
#include "modules/frame.h"
#include "modules/big_button.h"
#include "modules/memory.h"
#include "modules/morse.h"
#include "modules/simon.h"
#include "modules/password.h"
#include "modules/wires.h"

void resetAndPowerOffModules()
{
  timer.cancel();
  bombStarted = false;
  setSolvedModules(0, 0);

  basePowerOff();
  bigButtonPowerOff();
  framePowerOff();
  memoryPowerOff();
  morsePowerOff();
  passwordPowerOff();
  simonPowerOff();
}

void setup()
{
  for (int i = 4; i <= 15; i++)
  {
    pinMode(i, INPUT_PULLUP);
  }

  pinMode(16, OUTPUT);
  for (int i = 40; i <= 51; i++)
  {
    pinMode(i, OUTPUT);
  }
  pinMode(A0, INPUT_PULLUP);
  pinMode(A1, INPUT_PULLUP);
  pinMode(A2, INPUT_PULLUP);
  pinMode(A3, INPUT_PULLUP);
  pinMode(A4, INPUT_PULLUP);
  pinMode(A5, INPUT_PULLUP);
  pinMode(A6, INPUT_PULLUP);
  pinMode(A7, INPUT_PULLUP);
  pinMode(A8, INPUT_PULLUP);
  pinMode(A9, INPUT_PULLUP);
  pinMode(A10, INPUT_PULLUP);
  pinMode(A11, INPUT);
  pinMode(A12, INPUT);
  pinMode(A13, INPUT);
  pinMode(A14, INPUT);
  pinMode(A15, INPUT);

  Serial.begin(9600);

  while (!Serial)
  { // wait for serial port to connect. Needed for native USB port only
  }

  Wire.begin();
  if (!multiplexer.begin())
  {
    Serial.write((byte)0xF);
    Serial.println("COULD NOT CONNECT MULTIPLEXER");
    engageSerialWriteCooldown();
  }

  delay(10);

  resetAndPowerOffModules();
}

void loopSerialRead()
{
  if (!Serial.available())
    return;

  int type = Serial.read();
  switch (type)
  {
  case 0x0: // Reset
    resetAndPowerOffModules();
    break;
  case 0x1: // Init frame
  {
    while (Serial.available() < 6)
    {
    }
    char serialNumber[6];
    Serial.readBytes(serialNumber, 6);
    setSerialNumber(serialNumber);

    while (Serial.available() < 1)
    {
    }
    setIndicatorLED(Serial.read());

    while (Serial.available() < 3)
    {
    }
    char indicatorLetters[3];
    Serial.readBytes(indicatorLetters, 3);
    setIndicatorText(indicatorLetters);
    break;
  }
  case 0x2: // Init static modules
  {
    while (Serial.available() < 1)
    {
    }
    byte morseCodeIndex = Serial.read();
    morseInitRead(morseCodeIndex);

    while (Serial.available() < 2)
    {
    }
    byte bigKnobColorIndex = Serial.read();
    byte bigKnobTextIndex = Serial.read();
    bigButtonInit(bigKnobColorIndex, bigKnobTextIndex);
    wiresInit();
    passwordInit();
    memoryInit();
    break;
  }
  case 0x3: // Start
  {
    while (Serial.available() < 2)
    {
    }
    byte upperByte = Serial.read();
    byte lowerByte = Serial.read();
    unsigned short seconds = (upperByte << 8) + lowerByte;

    baseModuleInit(seconds);
    bigButtonStart();
    memoryStart();
    frameStart();

    startBomb();
    morseStart();
    break;
  }
  case 0x4: // Set Tries
  {
    while (Serial.available() < 1)
    {
    }
    byte tries = Serial.read();
    setTries(tries);
    break;
  }
  case 0x5: // Set Solved
  {
    while (Serial.available() < 1) // 2)
    {
    }
    byte data0 = Serial.read();
    // byte data1 = Serial.read();
    setSolvedModules(data0, 0);
    break;
  }
  case 0x6: // BigKnob Strip
  {
    while (Serial.available() < 1)
    {
    }
    byte colorIndex = Serial.read();
    bigButtonSetStripColor(colorIndex);
    break;
  }
  case 0x7: // Password Text
  {
    while (Serial.available() < 5)
    {
    }
    char passwordLetters[5];
    Serial.readBytes(passwordLetters, 5);
    setPassword(passwordLetters);
    break;
  }
  case 0x8: // SimonSays
  {
    while (Serial.available() < 2)
    {
    }
    byte data[2];
    Serial.readBytes(data, 2);
    simonInit(data[0], data[1]);
    break;
  }
  case 0x9: // Memory
  {
    while (Serial.available() < 2)
    {
    }
    byte data[2];
    Serial.readBytes(data, 2);
    memorySetNumber(data[0], data[1]);
    break;
  }
  case 0xE: // Resend serial data
  {
    resendSerialData();
    break;
  }
  }
}

void loop()
{
  timer.tick();

  loopSerialRead();

  if (!bombStarted)
  {
    return;
  }

  baseModuleLogicLoop();

  simonLogicLoop();
  morseLogicLoop();

  if (!loopLogicButtonCooldown)
  {
    morseLogicButtonLoop();
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
