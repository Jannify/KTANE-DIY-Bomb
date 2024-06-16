#include "common.h"
#include "modules/memory.h"

bool memoryButtonWasPressed = false;

void memoryInit()
{
    memoryTriesBuffer &= 0b11100000;
    shiftOutLED(OUTPUT_Clock_MemoryTriesIndicator, memoryTriesBuffer);
}

void memorySetNumber(byte data0, byte data1)
{
    int bigNumber = (data0 & 0b11000000) >> 6;
    int smallNumber1 = (data0 & 0b00110000) >> 4;
    int smallNumber2 = (data0 & 0b00001100) >> 2;
    int smallNumber3 = (data0 & 0b00000011);
    int smallNumber4 = (data1 & 0b11000000) >> 6;
    
    shiftOut(OUTPUT_Register_Data_Memory, OUTPUT_Clock_Memory_Big, LSBFIRST, bigNumber);
    shiftOut(OUTPUT_Register_Data_Memory, OUTPUT_Clock_Memory_Left, LSBFIRST, smallNumber1 | (smallNumber2 << 4));
    shiftOut(OUTPUT_Register_Data_Memory, OUTPUT_Clock_Memory_Right, LSBFIRST, smallNumber3 | (smallNumber4 << 4));
}

void memoryLogicLoop()
{
}

void memoryLogicButtonLoop()
{
}

void memorySerialWriteLoop()
{
    byte memoryButtonPressed = -1;
    if (digitalRead(INPUT_Memory_1))
    {
        memoryButtonPressed = 0;
    }
    else if (digitalRead(INPUT_Memory_2))
    {
        memoryButtonPressed = 1;
    }
    else if (digitalRead(INPUT_Memory_3))
    {
        memoryButtonPressed = 2;
    }
    else if (digitalRead(INPUT_Memory_4))
    {
        memoryButtonPressed = 3;
    }
    if (memoryButtonPressed == -1)
    {
        memoryButtonWasPressed = false;
    }
    else if (!memoryButtonWasPressed)
    {
        memoryButtonWasPressed = true;
        Serial.write((byte)0x5);
        Serial.write(memoryButtonPressed);
        engageSerialWriteCooldown();
    }
}
