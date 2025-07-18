#include "common.h"

Timer<20> timer;

TCA9548 multiplexer(0x70);

bool activeModules[11] = {false};
bool solvedModules[11] = {false};
bool bombStarted = false;

bool loopLogicButtonCooldown = false;
bool loopSerialWriteCooldown = false;

byte memoryTriesBuffer = 0x00;
byte lastSendLength = 0;
byte lastSendData[4] = {0x0};

byte receivedData[10] = {0xFF};

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
  timer.in(200, [](void *) -> bool
           {
      loopLogicButtonCooldown = false;
      return false; });
}

void engageSerialWriteCooldown()
{
  loopSerialWriteCooldown = true;
  timer.in(200, [](void *) -> bool
           {
      loopSerialWriteCooldown = false;
      return false; });
}

void setActiveModules(byte data0, byte data1)
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
}

void setSolvedModules(byte data0, byte data1)
{
  solvedModules[0] = data0 & 0b00000001;
  solvedModules[1] = data0 & 0b00000010;
  solvedModules[2] = data0 & 0b00000100;
  solvedModules[3] = data0 & 0b00001000;
  solvedModules[4] = data0 & 0b00010000;
  solvedModules[5] = data0 & 0b00100000;
  solvedModules[6] = data0 & 0b01000000;

  // solvedModules[7] = data1 & 0b00000001;
  // solvedModules[8] = data1 & 0b00000010;
  // solvedModules[9] = data1 & 0b00000100;
  // solvedModules[10] = data1 & 0b00001000;
  // solvedModules[11] = data1 & 0b00010000;

  shiftOutLED(OUTPUT_Clock_ModulesSolved_1, data0);
  // shiftOutLED(OUTPUT_Clock_ModulesSolved_2, data1);
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

void sendSerialDataWithChecksum()
{
  byte calc_checksum = 0;
  for (size_t i = 0; i < lastSendLength; i++)
  {
    Serial.write(lastSendData[i]);
    calc_checksum ^= lastSendData[i];
  }
  calc_checksum = ~calc_checksum;
  Serial.write(calc_checksum);
}

void sendSerialData(byte type)
{
  lastSendLength = 1;
  lastSendData[0] = type;

  sendSerialDataWithChecksum();
  engageSerialWriteCooldown();
}

void sendSerialData(byte type, byte data)
{
  lastSendLength = 2;
  lastSendData[0] = type;
  lastSendData[1] = data;

  sendSerialDataWithChecksum();
  engageSerialWriteCooldown();
}

void sendSerialData(byte type, byte data0, byte data1)
{
  lastSendLength = 3;
  lastSendData[0] = type;
  lastSendData[1] = data0;
  lastSendData[2] = data1;

  sendSerialDataWithChecksum();
  engageSerialWriteCooldown();
}

void sendSerialData(byte type, byte data0, byte data1, byte data2)
{
  lastSendLength = 4;
  lastSendData[0] = type;
  lastSendData[1] = data0;
  lastSendData[2] = data1;
  lastSendData[3] = data2;

  sendSerialDataWithChecksum();
  engageSerialWriteCooldown();
}

void resendSerialData()
{
  sendSerialDataWithChecksum();
  engageSerialWriteCooldown();
}

bool receiveSerialData(byte type, byte length) {
  while (Serial.available() < length + 1)
  {
  }

  byte calc_checksum = type;
  for (byte i = 0; i < length; i++)
  {
    receivedData[i] = Serial.read();
    calc_checksum ^= receivedData[i];
  }
  calc_checksum = ~calc_checksum;
  
  byte received_checksum = Serial.read();

  return calc_checksum == received_checksum;
}