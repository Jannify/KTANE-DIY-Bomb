#include "common.h"
#include "modules/password.h"

#include <U8g2lib.h>

U8X8_SSD1309_128X64_NONAME2_HW_I2C greenDisplay;
char password[5];

bool passwordButtonWasPressed = false;

void passwordInit()
{
    multiplexer.selectChannel(MULTIPLEXER_Password);
    greenDisplay.begin();
    greenDisplay.setPowerSave(1);
    greenDisplay.setInverseFont(1);
    greenDisplay.setFont(u8x8_font_profont29_2x3_f);
    greenDisplay.fillDisplay();
    greenDisplay.clearLine(0);
    greenDisplay.clearLine(1);
    greenDisplay.clearLine(62);
    greenDisplay.clearLine(63);
}

void setPassword(char *newPassword)
{
    memcpy(password, newPassword, sizeof(char) * 5);

    if (bombStarted)
    {
        showPassword();
    }
}

uint8_t full_tile[8] = {255, 255, 255, 255, 255, 255, 255, 255};

void showPassword()
{
    if (password[0] == (char)0xFF && password[1] == (char)0xFF &&
        password[2] == (char)0xFF && password[3] == (char)0xFF &&
        password[4] == (char)0xFF)
    {
        passwordPowerOff();
        return;
    }

    multiplexer.selectChannel(MULTIPLEXER_Password);
    greenDisplay.setPowerSave(0);
    // greenDisplay.drawTile(0, 4, 16, full_tile);
    for (byte i = 0; i < 5; i++)
    {
        greenDisplay.drawGlyph(i * 3 + 1, 4, password[i]);
    }
}

void passwordSerialWriteLoop()
{
    if (!activeModules[5] || solvedModules[5])
    {
        return;
    }

    bool pressedSend = !digitalRead(INPUT_Pass_Send);

    bool wasButtonUp = false;
    byte buttonIndex = 255;
    if (!digitalRead(INPUT_Pass_1_Up))
    {
        wasButtonUp = true;
        buttonIndex = 0;
    }
    else if (!digitalRead(INPUT_Pass_1_Down))
    {
        buttonIndex = 0;
    }
    else if (!digitalRead(INPUT_Pass_2_Up))
    {
        wasButtonUp = true;
        buttonIndex = 1;
    }
    else if (!digitalRead(INPUT_Pass_2_Down))
    {
        buttonIndex = 1;
    }
    else if (!digitalRead(INPUT_Pass_3_Up))
    {
        wasButtonUp = true;
        buttonIndex = 2;
    }
    else if (!digitalRead(INPUT_Pass_3_Down))
    {
        buttonIndex = 2;
    }
    else if (!digitalRead(INPUT_Pass_4_Up))
    {
        wasButtonUp = true;
        buttonIndex = 3;
    }
    else if (!digitalRead(INPUT_Pass_4_Down))
    {
        buttonIndex = 3;
    }
    else if (!digitalRead(INPUT_Pass_5_Up))
    {
        wasButtonUp = true;
        buttonIndex = 4;
    }
    else if (!digitalRead(INPUT_Pass_5_Down))
    {
        buttonIndex = 4;
    }

    if (!pressedSend && buttonIndex == 255)
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
        else if (buttonIndex != 255)
        {
            byte data = wasButtonUp ? 0b01000000 : 0x0;
            data |= buttonIndex << 3;
            sendSerialData(0x3, data);
        }
    }
}

void passwordPowerOff()
{
    multiplexer.selectChannel(MULTIPLEXER_Password);
    greenDisplay.begin();
    greenDisplay.clearDisplay();
    greenDisplay.setPowerSave(1);
}