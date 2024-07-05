#include "modules/frame.h"

#include <U8g2lib.h>

U8X8_SSD1306_128X64_NONAME_HW_I2C indicatorText;
U8G2_SH1107_64X128_1_HW_I2C serialNumberText(U8G2_R1);

bool valueIndicatorLED;

void setIndicatorLED(bool value)
{
  valueIndicatorLED = value;
  if(bombStarted) {
    digitalWrite(OUTPUT_Indicator_LED, value);
  }
}

void setIndicatorText(char *txt)
{
  multiplexer.selectChannel(MULTIPLEXER_Indicator);
  indicatorText.begin();
  indicatorText.setFont(u8x8_font_inr46_4x8_r);
  indicatorText.setPowerSave(1);
  for (size_t i = 0; i < 3; i++)
  {
    indicatorText.drawGlyph(6 * i, 0, txt[i]);
  }
}

void setSerialNumber(char *txt)
{
  return;
  multiplexer.selectChannel(MULTIPLEXER_SerialNumber);
  serialNumberText.begin();
  serialNumberText.setFont(u8g2_font_inb16_mr);
  serialNumberText.setPowerSave(1);
  serialNumberText.firstPage();
  do
  {
    serialNumberText.drawStr(0, 20, "Serien");
    serialNumberText.drawStr(90, 20, "Nr:");
    serialNumberText.drawBox(0, 28, 128, 4);
    for (size_t i = 0; i < 6; i++)
    {
      serialNumberText.drawGlyph(22 * i, 55, txt[i]);
    }
  } while (serialNumberText.nextPage());
}

void frameStart()
{
  digitalWrite(OUTPUT_Indicator_LED, valueIndicatorLED);
  multiplexer.selectChannel(MULTIPLEXER_Indicator);
  indicatorText.setPowerSave(0);

  // multiplexer.selectChannel(MULTIPLEXER_SerialNumber);
  // serialNumberText.setPowerSave(0);
}

void framePowerOff()
{
  multiplexer.selectChannel(MULTIPLEXER_Indicator);
  indicatorText.clearDisplay();

  // multiplexer.selectChannel(MULTIPLEXER_SerialNumber);
  // serialNumberText.clearDisplay();
  setIndicatorLED(false);
}