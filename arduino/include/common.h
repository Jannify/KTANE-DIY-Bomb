#pragma once

#include "TCA9548.h"
#include <arduino-timer.h>
#include <Arduino.h>

#define INPUT_WIRE_1 A0
#define INPUT_WIRE_2 A1
#define INPUT_WIRE_3 A2
#define INPUT_WIRE_4 A3
#define INPUT_WIRE_5 A4
#define INPUT_WIRE_6 A5
#define INPUT_BigButton A6
#define INPUT_Simon_Blue A7
#define INPUT_Simon_Red A8
#define INPUT_Simon_Yellow A9
#define INPUT_Simon_Green A10
#define INPUT_Memory_1 A11
#define INPUT_Memory_2 A12
#define INPUT_Memory_3 A13
#define INPUT_Memory_4 A14
#define INPUT_Morse_Send A15
#define INPUT_Morse_Left 37
#define INPUT_Morse_Right 38
#define INPUT_Pass_Send 39
#define INPUT_Pass_1_Up 40
#define INPUT_Pass_2_Up 41
#define INPUT_Pass_3_Up 42
#define INPUT_Pass_4_Up 43
#define INPUT_Pass_5_Up 44
#define INPUT_Pass_1_Down 45
#define INPUT_Pass_2_Down 46
#define INPUT_Pass_3_Down 47
#define INPUT_Pass_4_Down 48
#define INPUT_Pass_5_Down 49

#define OUTPUT_BigButton_Color 2
#define OUTPUT_BigButton_Strip 3
#define OUTPUT_7Bit_Data 4
#define OUTPUT_7Bit_Clock_0 5
#define OUTPUT_7Bit_Clock_1 6
#define OUTPUT_7Bit_Clock_2 7
#define OUTPUT_7Bit_Clock_3 8
#define OUTPUT_7Bit_Clock_4 9
#define OUTPUT_Memory_Lvl_1 10
#define OUTPUT_Memory_Lvl_2 11
#define OUTPUT_Memory_Lvl_3 12
#define OUTPUT_Memory_Lvl_4 13
#define OUTPUT_Simon_Blue 14  // Needs to be in succession
#define OUTPUT_Simon_Red 15
#define OUTPUT_Simon_Yellow 16
#define OUTPUT_Simon_Green 17
#define OUTPUT_Tries_1 18
#define OUTPUT_Tries_2 19
#define OUTPUT_Module_0 22
#define OUTPUT_Module_1 23
#define OUTPUT_Module_2 24
#define OUTPUT_Module_3 25
#define OUTPUT_Module_4 26
#define OUTPUT_Module_5 27
#define OUTPUT_Module_6 28
#define OUTPUT_Module_7 29
#define OUTPUT_Module_8 30
#define OUTPUT_Module_9 31
#define OUTPUT_Module_10 32
#define OUTPUT_RESET 33
#define OUTPUT_Indicator 34
#define OUTPUT_Morse_LED 35

#define I2C_SDA 20
#define I2C_SCL 21
#define SPI_CIPO 50
#define SPI_COPI 51
#define SPI_SCK 52
#define SPI_SS 53

extern TCA9548 multiplexer;
extern Timer<20> timer;

extern bool loopLogicButtonCooldown;
extern bool loopSerialWriteCooldown;

void engageLogicCooldown();
void engageSerialWriteCooldown();
bool setPinLow(void *argument);
void reset();