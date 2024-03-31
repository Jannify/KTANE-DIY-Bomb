#include "common.h"
#include "modules/big_button.h"

#include <Adafruit_NeoPixel.h>
#include <U8g2lib.h>

Adafruit_NeoPixel bigKnob = Adafruit_NeoPixel(7, OUTPUT_BigButton_Color, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel bigKnobStrip = Adafruit_NeoPixel(1, OUTPUT_BigButton_Strip, NEO_GRB + NEO_KHZ800);
U8X8_SSD1306_128X32_UNIVISION_HW_I2C buttonDisplay;

bool wasButtonPressed = false;
unsigned long startPressed = 0;
unsigned long endPressed = 0;

void bigButtonInitRead()
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
    byte bigKnobTextLength = Serial.read(); // TODO: THIS IS NOT UP TO DATE ANYMORE
    while (Serial.available() < bigKnobTextLength)
    {
    }
    char bigKnobText[10];
    Serial.readBytes(bigKnobText, bigKnobTextLength);
    // TODO: Set bigKnobText to screen

    multiplexer.selectChannel(3);
    buttonDisplay.begin();
    buttonDisplay.setFont(u8x8_font_px437wyse700a_2x2_r);
    buttonDisplay.drawString(0, 2, bigKnobText);
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