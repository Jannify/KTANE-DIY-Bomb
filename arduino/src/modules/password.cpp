#include "common.h"
#include "modules/password.h"

#include <U8g2lib.h>

U8X8_SSD1309_128X64_NONAME0_HW_I2C greenDisplay;

bool passwordButtonWasPressed = false;

void passwordInit()
{
    return;
    multiplexer.selectChannel(MULTIPLEXER_Password);
    greenDisplay.begin();
    greenDisplay.setFont(u8x8_font_profont29_2x3_r);
    greenDisplay.setInverseFont(1);
}

void setPassword(char *password)
{
    return;
    multiplexer.selectChannel(MULTIPLEXER_Password);
    greenDisplay.fillDisplay();
    for (size_t i = 0; i < 5; i++)
    {
        greenDisplay.drawGlyph(i * 3 + 1, 3, password[i]);
    }
}

void passwordSerialWriteLoop()
{
    return;
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
            sendSerialData(0x3, 0b10000000);
        }
        else if (buttonIndex != -1)
        {
            byte data = wasButtonUp ? 0b01000000 : 0x0;
            data |= buttonIndex << 3;
            sendSerialData(0x3, data);
        }
    }
}

void passwordPowerOff()
{
    return;
    multiplexer.selectChannel(MULTIPLEXER_Password);
    greenDisplay.begin();
    greenDisplay.clearDisplay();
    greenDisplay.setPowerSave(1);
}