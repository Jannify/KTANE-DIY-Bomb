#include "modules/frame.h"

#include <U8g2lib.h>

U8X8_SSD1306_128X64_NONAME_HW_I2C indicatorText;

void setIndicatorText(char c1, char c2, char c3)
{
  multiplexer.selectChannel(0);
  indicatorText.begin();
  indicatorText.setFont(u8x8_font_inr46_4x8_r);
  indicatorText.drawGlyph(0, 0, c1);
  indicatorText.drawGlyph(6, 0, c2);
  indicatorText.drawGlyph(12, 0, c3);
}
