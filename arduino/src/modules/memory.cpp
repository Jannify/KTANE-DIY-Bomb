#include "common.h"
#include "modules/memory.h"


bool memoryButtonWasPressed = false;

void memoryInit(byte data0, byte data1)
{
    // int bigNumber = (data0 & 0b11000000) >> 6;
    // int smallNumber1 = (data0 & 0b00110000) >> 4;
    // int smallNumber2 = (data0 & 0b00001100) >> 2;
    // int smallNumber3 = (data0 & 0b00000011);
    // int smallNumber4 = (data1 & 0b11000000) >> 6;
    int level = (data1 & 0b00111000) >> 3;

    digitalWrite(OUTPUT_Memory_Lvl_1, LOW);
    digitalWrite(OUTPUT_Memory_Lvl_2, LOW);
    digitalWrite(OUTPUT_Memory_Lvl_3, LOW);
    digitalWrite(OUTPUT_Memory_Lvl_4, LOW);
    if (level >= 4)
    {
        digitalWrite(OUTPUT_Memory_Lvl_4, HIGH);
    }
    if (level >= 3)
    {
        digitalWrite(OUTPUT_Memory_Lvl_3, HIGH);
    }
    if (level >= 2)
    {
        digitalWrite(OUTPUT_Memory_Lvl_2, HIGH);
    }
    if (level >= 1)
    {
        digitalWrite(OUTPUT_Memory_Lvl_1, HIGH);
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
    int memoryButtonPressed = -1;
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
        Serial.write(0x5);
        Serial.write(memoryButtonPressed);
        engageSerialWriteCooldown();
    }
}
