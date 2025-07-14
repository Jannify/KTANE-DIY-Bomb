#include "common.h"
#include "modules/base.h"
#include "modules/big_button.h"

#include <Adafruit_NeoPixel.h>
#include <U8g2lib.h>

const unsigned long PRESS_THRESHOLD_MS = 500;

const char _TEXT_ABRECHEN[] PROGMEM = "Abbrechen";
const char _TEXT_HALTEN[] PROGMEM = "Gedrückt";
const char _TEXT_SPRENGEN[] PROGMEM = "Sprengen";
const char _TEXT_DRUECKEN[] PROGMEM = "Drücken";
const char _TEXT_HALTEN2[] PROGMEM = "Halten";
const char *TEXT_TABLE[] = {_TEXT_ABRECHEN, _TEXT_HALTEN, _TEXT_SPRENGEN, _TEXT_DRUECKEN, _TEXT_HALTEN2};
const u8g2_uint_t _TEXT_X_OFFSET[] = {12, 20, 19, 23, 32}; // (display_width - txt_width) / 2

Adafruit_NeoPixel bigKnob = Adafruit_NeoPixel(7, OUTPUT_BigButton_Color, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel bigKnobStrip = Adafruit_NeoPixel(8, OUTPUT_BigButton_Strip, NEO_GRB + NEO_KHZ800);
U8G2_SSD1306_128X32_UNIVISION_1_HW_I2C buttonDisplay(U8G2_R0);

uint32_t bigKnobColor;
uint32_t bigKnobStripColor;

bool wasButtonPressed = false;
unsigned long startPressed = 0;
unsigned long endPressed = 0;

void bigButtonInit(byte bigKnobColorIndex, byte textIndex)
{
    return;
    switch (bigKnobColorIndex)
    {
    case 0: // Red
        bigKnobColor = 0xFF0000;
        break;
    case 1: // Blue
        bigKnobColor = 0x0000FF;
        break;
    case 2: // Yellow
        bigKnobColor = 0xFFFF00;
        break;
    case 3: // White
        bigKnobColor = 0xFFFFFF;
        break;
    }

    multiplexer.selectChannel(MULTIPLEXER_BigButton);
    buttonDisplay.begin();
    buttonDisplay.enableUTF8Print();
    buttonDisplay.setFont(u8g2_font_luRS14_tf);
    buttonDisplay.setPowerSave(1);

    buttonDisplay.firstPage();
    do
    {
        if (textIndex == 1)
        {
            buttonDisplay.setCursor(_TEXT_X_OFFSET[1], 14);
            buttonDisplay.print((class __FlashStringHelper *)TEXT_TABLE[1]);
            buttonDisplay.setCursor(_TEXT_X_OFFSET[4], 32);
            buttonDisplay.print((class __FlashStringHelper *)TEXT_TABLE[4]);
        }
        else
        {
            buttonDisplay.setCursor(_TEXT_X_OFFSET[textIndex], 25);
            buttonDisplay.print((class __FlashStringHelper *)TEXT_TABLE[textIndex]);
        }
    } while (buttonDisplay.nextPage());
}

void bigButtonStart()
{
    return;
    multiplexer.selectChannel(MULTIPLEXER_BigButton);
    buttonDisplay.setPowerSave(0);

    bigKnob.begin();
    bigKnob.fill(bigKnobColor, 0, 7);
    bigKnob.show();

    bigKnobStrip.begin();
}

void bigButtonSetStripColor(byte bigKnobColorIndex)
{
    return;
    switch (bigKnobColorIndex)
    {
    case 0: // Red
        bigKnobStripColor = 0xFF0000;
        break;
    case 1: // Blue
        bigKnobStripColor = 0x0000FF;
        break;
    case 2: // Yellow
        bigKnobStripColor = 0xFFFF00;
        break;
    case 3: // White
        bigKnobStripColor = 0xFFFFFF;
        break;
    }
}

void bigButtonSerialWriteLoop()
{
    return;
    if (!activeModules[0] || solvedModules[0])
    {
        return;
    }

    if (!wasButtonPressed && !digitalRead(INPUT_BigButton))
    {
        startPressed = millis();
        wasButtonPressed = true;

        bigKnobStrip.fill(bigKnobStripColor, 0, 8);
        bigKnobStrip.show();
    }

    if (wasButtonPressed && digitalRead(INPUT_BigButton))
    {
        endPressed = millis();
        wasButtonPressed = false;

        bigKnobStrip.clear();
        bigKnobStrip.show();

        unsigned long difference = endPressed - startPressed;
        bool wasLongPress = difference <= PRESS_THRESHOLD_MS ? false : true;
        sendSerialData(0x1, wasLongPress, lastSecondsLeft, lastSecondsLeft >> 8);
    }
}

void bigButtonPowerOff()
{
    return;
    bigKnob.begin();
    bigKnob.clear();
    bigKnob.show();

    bigKnobStrip.begin();
    bigKnobStrip.clear();
    bigKnobStrip.show();

    multiplexer.selectChannel(MULTIPLEXER_BigButton);
    buttonDisplay.begin();
    buttonDisplay.clearDisplay();
    buttonDisplay.setPowerSave(1);
}