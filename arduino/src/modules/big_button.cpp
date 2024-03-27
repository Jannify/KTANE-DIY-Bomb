#include <Adafruit_NeoPixel.h>

#include "common.h"
#include "modules/big_button.h"

Adafruit_NeoPixel bigKnob = Adafruit_NeoPixel(7, OUTPUT_BigButton_Color, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel bigKnobStrip = Adafruit_NeoPixel(1, OUTPUT_BigButton_Strip, NEO_GRB + NEO_KHZ800);

bool wasButtonPressed = false;
unsigned long startPressed = 0;
unsigned long endPressed = 0;

void bigButtonInit()
{
    while (Serial.available() < 3)
    {
    }
    byte bigKnobColor[3];
    Serial.readBytes(bigKnobColor, 3);
    for (int i = 0; i <= 7; i++)
    {
        bigKnob.setPixelColor(i, bigKnobColor[0], bigKnobColor[1],
                              bigKnobColor[2]);
    }
    bigKnob.show();

    while (Serial.available() < 1)
    {
    }
    byte bigKnobTextLength = Serial.read();
    while (Serial.available() < bigKnobTextLength)
    {
    }
    byte bigKnobText[10];
    Serial.readBytes(bigKnobText, bigKnobTextLength);
    // TODO: Set bigKnobText to screen
}

void bigButtonUpdateStrip(byte *color)
{
    for (int i = 0; i <= 1; i++)
    {
        bigKnobStrip.setPixelColor(i, color[0], color[1], color[2]);
    }
    bigKnobStrip.show();
}

void bigButtonSerialWriteLoop()
{
    if (!wasButtonPressed && digitalRead(INPUT_BigButton))
    {
        startPressed = millis();
        wasButtonPressed = true;
    }

    if (wasButtonPressed && !digitalRead(INPUT_BigButton))
    {
        endPressed = millis();
        wasButtonPressed = false;

        byte difference = endPressed - startPressed;
        Serial.write(0x1);
        Serial.write(difference);
        engageSerialWriteCooldown();
    }
}