#include "modules/frame.h"

#include <U8g2lib.h>

U8X8_SSD1306_128X64_NONAME_HW_I2C indicatorText;
U8G2_SH1107_64X128_1_HW_I2C serialNumberText(U8G2_R1);

void setIndicatorText(char *txt)
{
  multiplexer.selectChannel(0);
  indicatorText.begin();
  indicatorText.setFont(u8x8_font_inr46_4x8_r);
  for (size_t i = 0; i < 3; i++)
  {
    indicatorText.drawGlyph(6 * i, 0, txt[i]);
  }
}

void setSerialNumber(char *txt)
{
  multiplexer.selectChannel(4);
  serialNumberText.begin();
  serialNumberText.setFont(u8g2_font_inb16_mr);
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

void setSolvedLEDs(byte data0, byte data1)
{
  shiftOutLED(OUTPUT_Clock_ModulesSolved_1, data0);
  shiftOutLED(OUTPUT_Clock_ModulesSolved_2, data1);
}
