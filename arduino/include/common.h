#pragma once

#include "TCA9548.h"
#include <arduino-timer.h>
#include <Arduino.h>

#define INPUT_Simon_Blue A2
#define INPUT_Simon_Red A0
#define INPUT_Simon_Yellow A3
#define INPUT_Simon_Green A1
#define INPUT_Morse_Send A4
#define INPUT_Morse_Left A5
#define INPUT_Morse_Right A6
#define INPUT_Memory_1 A10
#define INPUT_Memory_2 A9
#define INPUT_Memory_3 A8
#define INPUT_Memory_4 A7
#define INPUT_WIRE_1 A12
#define INPUT_WIRE_2 A12
#define INPUT_WIRE_3 A13
#define INPUT_WIRE_4 A14
#define INPUT_WIRE_5 A15

#define OUTPUT_BigButton_Strip 2
#define OUTPUT_BigButton_Color 3

#define INPUT_Pass_1_Up 4
#define INPUT_Pass_2_Up 5
#define INPUT_Pass_3_Up 6
#define INPUT_Pass_4_Up 7
#define INPUT_Pass_5_Up 8
#define INPUT_Pass_1_Down 9
#define INPUT_Pass_2_Down 10
#define INPUT_Pass_3_Down 11
#define INPUT_Pass_4_Down 12
#define INPUT_Pass_5_Down 13
#define INPUT_Pass_Send 14
#define INPUT_BigButton 15

#define OUTPUT_Morse_LED A11

#define OUTPUT_Indicator_LED 41
#define OUTPUT_Register_Data_Memory 42
#define OUTPUT_Clock_Memory_Left 44
#define OUTPUT_Clock_Memory_Right 43
#define OUTPUT_Clock_Memory_Big 45
#define OUTPUT_Clock_MemoryTriesIndicator 46
#define OUTPUT_Register_Data_LED 47
#define OUTPUT_Clock_SimonSays 48
#define OUTPUT_Clock_ModulesSolved_1 49
#define OUTPUT_Clock_ModulesSolved_2 50
#define OUTPUT_BUZZER 40
#define OUTPUT_ClockDisplay_Data 39
#define OUTPUT_ClockDisplay_Clk 38

#define I2C_SDA 20
#define I2C_SCL 21

#define MULTIPLEXER_SerialNumber 0
#define MULTIPLEXER_Morse 1
#define MULTIPLEXER_Indicator 2
#define MULTIPLEXER_BigButton 3
#define MULTIPLEXER_Password 4

extern TCA9548 multiplexer;
extern Timer<20> timer;

extern bool activeModules[];
extern bool solvedModules[];
extern bool bombStarted;

extern bool loopLogicButtonCooldown;
extern bool loopSerialWriteCooldown;
extern byte receivedData[10];

extern byte memoryTriesBuffer;

void startBomb();
void engageLogicCooldown();
void engageSerialWriteCooldown();
void setActiveModules(byte data0, byte data1);
void setSolvedModules(byte data0, byte data1);
bool setPinLow(void *argument);
void shiftOutLED(byte clockPin, byte val);

void sendSerialData(byte type);
void sendSerialData(byte type, byte data);
void sendSerialData(byte type, byte data0, byte data1);
void sendSerialData(byte type, byte data0, byte data1, byte data2);
void resendSerialData();

bool receiveSerialData(byte type, byte length);