#include "common.h"
#include "modules/password.h"

#include <U8g2lib.h>

U8X8_SSD1309_128X64_NONAME0_HW_I2C greenDisplay;

bool passwordButtonWasPressed = false;

void passwordInit()
{
    multiplexer.selectChannel(2);
    greenDisplay.begin();
    greenDisplay.setFont(u8x8_font_profont29_2x3_r);
    greenDisplay.setInverseFont(1);
}

void setPassword(char *password)
{
    multiplexer.selectChannel(2);
    greenDisplay.fillDisplay();
    for (size_t i = 0; i < 5; i++)
    {
        greenDisplay.drawGlyph(i * 3 + 1, 3, password[i]);
    }
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
            Serial.write((byte)0x3);
            Serial.write((byte)0b10000000);
            engageSerialWriteCooldown();
        }
        else if (buttonIndex != -1)
        {
            byte data = wasButtonUp ? 0b01000000 : 0x0;
            data = data | buttonIndex << 3;
            Serial.write((byte)0x3);
            Serial.write(data);
            engageSerialWriteCooldown();
        }
    }
}