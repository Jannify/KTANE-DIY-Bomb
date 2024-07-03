#include "common.h"
#include "modules/base.h"
#include "modules/frame.h"
#include "modules/big_button.h"
#include "modules/memory.h"
#include "modules/morse.h"
#include "modules/simon.h"
#include "modules/password.h"
#include "modules/wires.h"

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

  Wire.begin();
  if (multiplexer.begin() == false)
  {
    Serial.write((byte)0xF);
    Serial.println("COULD NOT CONNECT MULTIPLEXER");
  }
}

void loopSerialRead()
{
  if (!Serial.available())
    return;

  int type = Serial.read();
  switch (type)
  {
  case 0: // Reset
    reset();
    break;
  case 1: // Init
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

    morseInitRead();
    bigButtonInitRead();
    wiresInit();
    passwordInit();
    memoryInit();
    break;
  }
  case 2: // Start
  {
    while (Serial.available() < 2)
    {
    }
    byte upperByte = Serial.read();
    byte lowerByte = Serial.read();
    unsigned short seconds = (upperByte << 8) + lowerByte;
    baseModuleInit(seconds);
    startBomb();
    morseStart();
    break;
  }
  case 3: // Set Tries
  {
    while (Serial.available() < 1)
    {
    }
    byte tries = Serial.read();
    setTries(tries);
    break;
  }
  case 4: // Set Solved
  {
    while (Serial.available() < 2)
    {
    }
    byte data0 = Serial.read();
    byte data1 = Serial.read();
    setSolvedModules(data0, data1);
    setSolvedLEDs(data0, data1);
    break;
  }
  case 5: // BigKnob Strip
  {
    while (Serial.available() < 1)
    {
    }
    byte colorIndex = Serial.read();
    bigButtonUpdateStrip(colorIndex);
    break;
  }
  case 6: // Password Text
  {
    while (Serial.available() < 5)
    {
    }
    char passwordLetters[5];
    Serial.readBytes(passwordLetters, 5);
    setPassword(passwordLetters);
    break;
  }
  case 7: // SimonSays
  {
    while (Serial.available() < 3)
    {
    }
    byte data[3];
    Serial.readBytes(data, 3);
    simonInit(data[0], data[1], data[2]);
    break;
  }
  case 8: // Memory
  {
    while (Serial.available() < 2)
    {
    }
    byte data[2];
    Serial.readBytes(data, 2);
    memorySetNumber(data[0], data[1]);
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

  return; // TESTING

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
    morseSerialWriteLoop();
  }
}
