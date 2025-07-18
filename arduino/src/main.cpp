#include "common.h"
#include "modules/base.h"
#include "modules/frame.h"
#include "modules/big_button.h"
#include "modules/memory.h"
#include "modules/morse.h"
#include "modules/simon.h"
#include "modules/password.h"
#include "modules/wires.h"
#include "main.h"
#include "test.h"

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
  pinMode(A11, OUTPUT); // is original input
  pinMode(A12, INPUT);
  pinMode(A13, INPUT);
  pinMode(A14, INPUT);
  pinMode(A15, INPUT);

  baseDisplayMsgOnClock("USB");

  Serial.begin(9600);

  while (!Serial)
  { // wait for serial port to connect. Needed for native USB port only
  }

  baseDisplayMsgOnClock("I2C");

  Wire.begin();
  if (!multiplexer.begin())
  {
    baseDisplayMsgOnClock("I2CF");
    Serial.write((byte)0xF);
    Serial.println("COULD NOT CONNECT MULTIPLEXER");
    engageSerialWriteCooldown();
  }

  delay(10);

  resetAndPowerOffModules();

  //test_setup();
}

void loop()
{
  timer.tick();
  //test_loop();
  //return;

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

void loopSerialRead()
{
  if (!Serial.available())
    return;

  int type = Serial.read();
  byte length = 255;

  switch (type)
  {
    // Length 0
    case 0x0:
    case 0xD:
    case 0xF:
      length = 0;
      break;
    // Length 1
    case 0x4:
    case 0x5:
    case 0x6:
      length = 1;
      break;
    // Length 2
    case 0x3:
    case 0x8:
    case 0x9:
      length = 2;
      break;
    // Length 4
    case 0x2:
      length = 4;
      break;
    // Length 5
    case 0x7:
      length = 5;
      break;
    // Length 10
    case 0x1:
      length = 10;
      break;
  }

  if(length==255) {
    Serial.write((byte)0xF);
    Serial.println("Got 255 length");
    return;
  }

  if (!receiveSerialData(type, length)) {
    sendSerialData(0xE);
    return;
  }
  
  switch (type)
  {
  case 0x0: // Reset
    resetAndPowerOffModules();
    break;
  case 0x1: // Init frame
  {
    char serialNumber[] = {receivedData[0], receivedData[1], receivedData[2], receivedData[3], receivedData[4], receivedData[5]};
    setSerialNumber(serialNumber);
    setIndicatorLED(receivedData[6]);
    char indicator[] = {receivedData[7], receivedData[8], receivedData[9]};
    setIndicatorText(indicator);
    break;
  }
  case 0x2: // Init static modules
  {
    setActiveModules(receivedData[0], 0);
    morseInitRead(receivedData[1]);
    bigButtonInit(receivedData[2], receivedData[3]);
    wiresInit();
    passwordInit();
    memoryInit();
    break;
  }
  case 0x3: // Start
  {
    unsigned short seconds = (receivedData[0] << 8) + receivedData[1];

    baseModuleInit(seconds);
    bigButtonStart();
    memoryStart();
    showPassword();
    frameStart();

    startBomb();
    morseStart();
    break;
  }
  case 0x4: // Set Tries
  {
    setTries(receivedData[0]);
    break;
  }
  case 0x5: // Set Solved
  {
    setSolvedModules(receivedData[0], 0);
    break;
  }
  case 0x6: // BigKnob Strip
  {
    bigButtonSetStripColor(receivedData[0]);
    break;
  }
  case 0x7: // Password Text
  {
    char passwordLetters[] = {receivedData[0], receivedData[1], receivedData[2], receivedData[3], receivedData[4]};
    setPassword(passwordLetters);
    break;
  }
  case 0x8: // SimonSays
  {
    simonInit(receivedData[0], receivedData[1]);
    break;
  }
  case 0x9: // Memory
  {
    memorySetNumber(receivedData[0], receivedData[1]);
    break;
  }
  case 0xD:
    bombSolved();
    break;
  case 0xE: // Resend serial data
  {
    resendSerialData();
    break;
  }
  case 0xF: // Setup bomb for exploded
  {
    explode();
    break;
  }
  //default: // Type was out of expected range

  }
}

void explode()
{
  timer.cancel();
  loopLogicButtonCooldown = false;
  loopSerialWriteCooldown = false;
  bombStarted = false;
  for (byte i = 0; i < 11; i++)
  {
    activeModules[i] = false;
  }
}

void bombSolved()
{
  timer.cancel();
  loopLogicButtonCooldown = false;
  loopSerialWriteCooldown = false;
  bombStarted = false;
}

void resetAndPowerOffModules()
{
  baseDisplayMsgOnClock("LOAD");

  timer.cancel();
  loopLogicButtonCooldown = false;
  loopSerialWriteCooldown = false;
  bombStarted = false;
  setSolvedModules(0, 0);

  baseDisplayMsgOnClock("LOD1");
  basePowerOff();
  baseDisplayMsgOnClock("LOD2");
  bigButtonPowerOff();
  baseDisplayMsgOnClock("LOD3");
  framePowerOff();
  baseDisplayMsgOnClock("LOA4");
  memoryPowerOff();
  baseDisplayMsgOnClock("LOA5");
  morsePowerOff();
  baseDisplayMsgOnClock("LOA6");
  passwordPowerOff();
  baseDisplayMsgOnClock("LOA7");
  simonPowerOff();

  baseDisplayMsgOnClock(" GO", true);
}
