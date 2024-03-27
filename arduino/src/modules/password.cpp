#include "common.h"
#include "modules/password.h"

bool passwordButtonWasPressed = false;

void passwordInit(byte *password)
{
    // TODO: Set passwordLetters to screen
}

void passwordSerialWriteLoop()
{
    bool pressedSend = digitalRead(INPUT_Pass_Send);

    bool wasButtonUp = false;
    int buttonIndex = -1;
    if (digitalRead(INPUT_Pass_1_Up))
    {
        wasButtonUp = true;
        buttonIndex = 0;
    }
    else if (digitalRead(INPUT_Pass_1_Down))
    {
        buttonIndex = 0;
    }
    else if (digitalRead(INPUT_Pass_2_Up))
    {
        wasButtonUp = true;
        buttonIndex = 1;
    }
    else if (digitalRead(INPUT_Pass_2_Down))
    {
        buttonIndex = 1;
    }
    else if (digitalRead(INPUT_Pass_3_Up))
    {
        wasButtonUp = true;
        buttonIndex = 2;
    }
    else if (digitalRead(INPUT_Pass_3_Down))
    {
        buttonIndex = 2;
    }
    else if (digitalRead(INPUT_Pass_4_Up))
    {
        wasButtonUp = true;
        buttonIndex = 3;
    }
    else if (digitalRead(INPUT_Pass_4_Down))
    {
        buttonIndex = 3;
    }
    else if (digitalRead(INPUT_Pass_5_Up))
    {
        wasButtonUp = true;
        buttonIndex = 4;
    }
    else if (digitalRead(INPUT_Pass_5_Down))
    {
        buttonIndex = 4;
    }

    if (!pressedSend && buttonIndex == -1)
    {
        passwordButtonWasPressed = false;
    }
    else if (!passwordButtonWasPressed)
    {
        passwordButtonWasPressed = true;

        if (pressedSend)
        {
            Serial.write(0x3);
            Serial.write(0b10000000);
            engageSerialWriteCooldown();
        }
        else if (buttonIndex != -1)
        {
            byte data = wasButtonUp ? 0b01000000 : 0x0;
            data = data | buttonIndex << 3;
            Serial.write(0x3);
            Serial.write(data);
            engageSerialWriteCooldown();
        }
    }
}