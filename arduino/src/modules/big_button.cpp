#include "common.h"
#include "modules/base.h"
#include "modules/big_button.h"

#include <Adafruit_NeoPixel.h>
#include <U8g2lib.h>

const unsigned long PRESS_THRESHOLD_MS = 500;

const char _TEXT_ABRECHEN[] PROGMEM = "Abbrechn";
const char _TEXT_HALTEN[] PROGMEM = "Gedruekt";
const char _TEXT_SPRENGEN[] PROGMEM = "Sprengen";
const char _TEXT_DRUECKEN[] PROGMEM = "Druecken";
const char _TEXT_HALTEN2[] PROGMEM = "Halten";
const char *const TEXT_TABLE[] PROGMEM = {_TEXT_ABRECHEN, _TEXT_HALTEN, _TEXT_SPRENGEN, _TEXT_DRUECKEN, _TEXT_HALTEN2};

Adafruit_NeoPixel bigKnob = Adafruit_NeoPixel(7, OUTPUT_BigButton_Color, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel bigKnobStrip = Adafruit_NeoPixel(8, OUTPUT_BigButton_Strip, NEO_GRB + NEO_KHZ800);
U8X8_SSD1306_128X32_UNIVISION_HW_I2C buttonDisplay;

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

    multiplexer.selectChannel(3);
    buttonDisplay.begin();
    buttonDisplay.setFont(u8x8_font_px437wyse700a_2x2_r);

    char txt[8];
    if(bigKnobTextIndex == 1) {
        strcpy_P(txt, (TEXT_TABLE[1]));
        buttonDisplay.drawString(0, 0, txt);
        strcpy_P(txt, (TEXT_TABLE[4]));
        buttonDisplay.drawString(0, 2, txt);
    } else {
        strcpy_P(txt, (TEXT_TABLE[bigKnobTextIndex]));
        buttonDisplay.drawString(0, 2, txt);
    }
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
