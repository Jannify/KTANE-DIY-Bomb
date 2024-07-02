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

bool wasButtonPressed = false;
unsigned long startPressed = 0;
unsigned long endPressed = 0;

void bigButtonInitRead()
{
    while (Serial.available() < 3)
    {
    }

    byte red, green, blue;
    byte bigKnobColorIndex = Serial.read();
    switch (bigKnobColorIndex)
    {
    case 0: // Red
        red = 255;
        green = 0;
        blue = 0;
        break;
    case 1: // Blue
        red = 0;
        green = 0;
        blue = 255;
        break;
    case 2: // Yellow
        red = 255;
        green = 255;
        blue = 0;
        break;
    case 3: // White
        red = 255;
        green = 255;
        blue = 255;
        break;
    }
    bigKnob.begin();
    for (int i = 0; i <= 7; i++)
    {
        bigKnob.setPixelColor(i, red, green, blue);
    }
    bigKnob.show();

    while (Serial.available() < 1)
    {
    }
    byte bigKnobTextIndex = Serial.read();

    multiplexer.selectChannel(MULTIPLEXER_BigButton);
    buttonDisplay.begin();
    buttonDisplay.enableUTF8Print();
    buttonDisplay.setFont(u8g2_font_luRS14_tf);

    buttonDisplay.firstPage();
    do
    {
        if (bigKnobTextIndex == 1)
        {
            buttonDisplay.setCursor(_TEXT_X_OFFSET[1], 14);
            buttonDisplay.print((class __FlashStringHelper *)TEXT_TABLE[1]);
            buttonDisplay.setCursor(_TEXT_X_OFFSET[4], 32);
            buttonDisplay.print((class __FlashStringHelper *)TEXT_TABLE[4]);
        }
        else
        {
            buttonDisplay.setCursor(_TEXT_X_OFFSET[bigKnobTextIndex], 25);
            buttonDisplay.print((class __FlashStringHelper *)TEXT_TABLE[bigKnobTextIndex]);
        }
    } while (buttonDisplay.nextPage());
}

void bigButtonUpdateStrip(byte bigKnobColorIndex)
{
    byte red, green, blue;
    switch (bigKnobColorIndex)
    {
    case 0: // Red
        red = 255;
        green = 0;
        blue = 0;
        break;
    case 1: // Blue
        red = 0;
        green = 0;
        blue = 255;
        break;
    case 2: // Yellow
        red = 255;
        green = 255;
        blue = 0;
        break;
    case 3: // White
        red = 255;
        green = 255;
        blue = 255;
        break;
    }
    bigKnobStrip.begin();
    for (int i = 0; i <= 8; i++)
    {
        bigKnobStrip.setPixelColor(i, red, green, blue);
    }
    bigKnobStrip.show();
}

void bigButtonSerialWriteLoop()
{
    if (!wasButtonPressed && !digitalRead(INPUT_BigButton))
    {
        startPressed = millis();
        wasButtonPressed = true;
    }

    if (wasButtonPressed && digitalRead(INPUT_BigButton))
    {
        endPressed = millis();
        wasButtonPressed = false;

        unsigned long difference = endPressed - startPressed;
        Serial.write((byte)0x1);
        Serial.write((byte)(difference <= PRESS_THRESHOLD_MS ? false : true));
        Serial.write((byte)(getSecondsLeft() >> 8));
        Serial.write((byte)getSecondsLeft());
        engageSerialWriteCooldown();
    }
}
