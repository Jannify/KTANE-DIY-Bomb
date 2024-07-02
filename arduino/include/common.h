#pragma once

#include "TCA9548.h"
#include <arduino-timer.h>
#include <Arduino.h>

#define INPUT_Simon_Blue A0
#define INPUT_Simon_Red A1
#define INPUT_Simon_Yellow A2
#define INPUT_Simon_Green A3
#define INPUT_Morse_Send A4
#define INPUT_Morse_Left A5
#define INPUT_Morse_Right A6
#define INPUT_Memory_1 A7
#define INPUT_Memory_2 A8
#define INPUT_Memory_3 A9
#define INPUT_Memory_4 A10
#define INPUT_WIRE_1 A11
#define INPUT_WIRE_2 A12
#define INPUT_WIRE_3 A13
#define INPUT_WIRE_4 A14
#define INPUT_WIRE_5 A15

#define OUTPUT_BigButton_Strip 2
#define OUTPUT_BigButton_Color 3

#define INPUT_BigButton 22
#define INPUT_Pass_Send 23
#define INPUT_Pass_1_Up 24
#define INPUT_Pass_2_Up 25
#define INPUT_Pass_3_Up 26
#define INPUT_Pass_4_Up 27
#define INPUT_Pass_5_Up 28
#define INPUT_Pass_1_Down 29
#define INPUT_Pass_2_Down 30
#define INPUT_Pass_3_Down 31
#define INPUT_Pass_4_Down 32
#define INPUT_Pass_5_Down 33

#define OUTPUT_Morse_LED 40

#define OUTPUT_Register_Data_Memory 42
#define OUTPUT_Clock_Memory_Left 43
#define OUTPUT_Clock_Memory_Right 44
#define OUTPUT_Clock_Memory_Big 45
#define OUTPUT_Clock_MemoryTriesIndicator 46
#define OUTPUT_Register_Data_LED 47
#define OUTPUT_Clock_SimonSays 48
#define OUTPUT_Clock_ModulesSolved_1 49
#define OUTPUT_Clock_ModulesSolved_2 50
#define OUTPUT_BUZZER 51
#define OUTPUT_ClockDisplay_Data 52
#define OUTPUT_ClockDisplay_Clk 53

#define I2C_SDA 20
#define I2C_SCL 21

#define MULTIPLEXER_SerialNumber 0
#define MULTIPLEXER_Morse 1
#define MULTIPLEXER_Indicator 2
#define MULTIPLEXER_BigButton 3
#define MULTIPLEXER_Password 4

extern TCA9548 multiplexer;
extern Timer<20> timer;

// BigButton, Wires, Morse, Password, SimonSays, Memory
extern bool activeModules[];
extern bool bombStarted;

extern bool loopLogicButtonCooldown;
extern bool loopSerialWriteCooldown;

extern byte memoryTriesBuffer;

void startBomb();
void engageLogicCooldown();
void engageSerialWriteCooldown();
void setSolvedModules(byte data0, byte data1);
bool setPinLow(void *argument);
void shiftOutLED(byte clockPin, byte val);
uint8_t pack8Bool(bool *a);
void reset();
