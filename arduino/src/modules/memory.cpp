#include "common.h"
#include "modules/memory.h"

bool memoryButtonWasPressed = false;

int bigNumber;
int smallNumber1;
int smallNumber2;
int smallNumber3;
int smallNumber4;

void memoryInit()
{
    memoryTriesBuffer &= 0b00000111;
    shiftOutLED(OUTPUT_Clock_MemoryTriesIndicator, memoryTriesBuffer);
}

void memoryStart()
{
    shiftOutLED(OUTPUT_Clock_MemoryTriesIndicator, memoryTriesBuffer);
    shiftOut(OUTPUT_Register_Data_Memory, OUTPUT_Clock_Memory_Big, MSBFIRST, bigNumber);
    shiftOut(OUTPUT_Register_Data_Memory, OUTPUT_Clock_Memory_Left, MSBFIRST, smallNumber2 | (smallNumber1 << 4));
    shiftOut(OUTPUT_Register_Data_Memory, OUTPUT_Clock_Memory_Right, MSBFIRST, smallNumber4 | (smallNumber3 << 4));
}

void memorySetNumber(byte data0, byte data1)
{
    bigNumber = 1 + ((data0 & 0b11000000) >> 6);
    smallNumber1 = 1 + ((data0 & 0b00110000) >> 4);
    smallNumber2 = 1 + ((data0 & 0b00001100) >> 2);
    smallNumber3 = 1 + (data0 & 0b00000011);
    smallNumber4 = 1 + ((data1 & 0b11000000) >> 6);
    byte memoryLevelValue = ((data1 & 0b00011110) << 2);
    memoryTriesBuffer = (memoryTriesBuffer & 0b10000111) | (memoryLevelValue & 0b01111000);

    if ((data1 & 0b00100000) > 0) // Level 5 complete
    {
        bigNumber = smallNumber1 = smallNumber2 = smallNumber3 = smallNumber4 = 0xFF;
    }

    if (bombStarted)
    {
        memoryStart();
    }
}

void memoryLogicLoop()
{
}

void memoryLogicButtonLoop()
{
}

void memorySerialWriteLoop()
{
    byte memoryButtonPressed = 255;
    if (!digitalRead(INPUT_Memory_1))
    {
        memoryButtonPressed = 0;
    }
    else if (!digitalRead(INPUT_Memory_2))
    {
        memoryButtonPressed = 1;
    }
    else if (!digitalRead(INPUT_Memory_3))
    {
        memoryButtonPressed = 2;
    }
    else if (!digitalRead(INPUT_Memory_4))
    {
        memoryButtonPressed = 3;
    }

    if (memoryButtonPressed == 255)
    {
        memoryButtonWasPressed = false;
    }
    else if (!memoryButtonWasPressed)
    {
        memoryButtonWasPressed = true;
        sendSerialData(0x5, memoryButtonPressed);
    }
}

void memoryPowerOff()
{
    memoryTriesBuffer &= 0b10000111;
    shiftOutLED(OUTPUT_Clock_MemoryTriesIndicator, memoryTriesBuffer);

    delay(2);
    shiftOut(OUTPUT_Register_Data_Memory, OUTPUT_Clock_Memory_Big, MSBFIRST, 0xFF);
    delay(2);
    shiftOut(OUTPUT_Register_Data_Memory, OUTPUT_Clock_Memory_Left, MSBFIRST, 0xFF);
    delay(2);
    shiftOut(OUTPUT_Register_Data_Memory, OUTPUT_Clock_Memory_Right, MSBFIRST, 0xFF);
    delay(2);
}