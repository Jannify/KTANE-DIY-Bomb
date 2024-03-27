#include "common.h"
#include "modules/base.h"
#include "modules/big_button.h"
#include "modules/memory.h"
#include "modules/morse.h"
#include "modules/simon.h"
#include "modules/password.h"
#include "modules/wires.h"

void setup()
{
  for (int i = 2; i <= 35; i++)
  {
    pinMode(i, OUTPUT);
  }
  for (int i = 37; i <= 49; i++)
  {
    pinMode(i, INPUT);
  }
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);
  pinMode(A6, INPUT);
  pinMode(A7, INPUT);
  pinMode(A8, INPUT);
  pinMode(A9, INPUT);
  pinMode(A10, INPUT);
  pinMode(A11, INPUT);
  pinMode(A12, INPUT);
  pinMode(A13, INPUT);
  pinMode(A14, INPUT);
  pinMode(A15, INPUT);

  digitalWrite(OUTPUT_RESET, HIGH);

  Serial.begin(9600);
}

void loopSerialRead()
{
  if (Serial.available())
  {
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
      byte serialNumber[6];
      Serial.readBytes(serialNumber, 6);
      // TODO: Set serialNumber to screen

      while (Serial.available() < 1)
      {
      }
      digitalWrite(OUTPUT_Indicator, (bool)Serial.read());

      while (Serial.available() < 3)
      {
      }
      byte indicatorLetters[3];
      Serial.readBytes(indicatorLetters, 3);

      morseInit();
      bigButtonInit();
      wiresInit();
      break;
    }
    case 2: // Start
    {
      while (Serial.available() < 2)
      {
      }
      byte minutes = Serial.read();
      byte seconds = Serial.read();
      baseModuleInit(minutes, seconds);
      break;
    }
    case 3: // Set Tries
    {
      if (lastTryTask != nullptr)
      {
        timer.cancel(lastTryTask);
        lastTryTask = nullptr;
      }
      while (Serial.available() < 1)
      {
      }
      byte tries = Serial.read();
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
      break;
    }
    case 4: // Set Solved
    {
      while (Serial.available() < 2)
      {
      }
      byte solvedModules1 = Serial.read();
      byte solvedModules2 = Serial.read();

      PORTA = solvedModules2;

      digitalWrite(OUTPUT_Module_8, LOW);
      digitalWrite(OUTPUT_Module_9, LOW);
      digitalWrite(OUTPUT_Module_10, LOW);
      if (solvedModules1 & B00000100)
      {
        digitalWrite(OUTPUT_Module_10, HIGH);
      }
      else if (solvedModules1 & B00000010)
      {
        digitalWrite(OUTPUT_Module_9, HIGH);
      }
      else if (solvedModules1 & B00000001)
      {
        digitalWrite(OUTPUT_Module_8, HIGH);
      }
      break;
    }
    case 5: // BigKnob Strip
    {
      while (Serial.available() < 3)
      {
      }
      byte bigKnobStripColor[3];
      Serial.readBytes(bigKnobStripColor, 3);
      bigButtonUpdateStrip(bigKnobStripColor);
      break;
    }
    case 6: // Password Text
    {
      while (Serial.available() < 5)
      {
      }
      byte passwordLetters[5];
      Serial.readBytes(passwordLetters, 5);
      passwordInit(passwordLetters);
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
      memoryInit(data[0], data[1]);
      break;
    }
    }
  }
}

void loop()
{
  timer.tick();

  loopSerialRead();

  if (!bombStarted)
  {
    // return;
  }

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
