#include "common.h"
#include "modules/morse.h"

#include <U8g2lib.h>

const char _MORSE_HALLE[] PROGMEM = ".... .- .-.. .-.. .s";
const char _MORSE_HALLO[] PROGMEM = ".... .- .-.. .-.. ---s";
const char _MORSE_LOKAL[] PROGMEM = ".-.. --- -.- .- .-..s";
const char _MORSE_STEAK[] PROGMEM = "... - . .- -.-s";
const char _MORSE_STUECK[] PROGMEM = "... - ..-- -.-. -.-s";
const char _MORSE_SPECK[] PROGMEM = "... .--. . -.-. -.-s";
const char _MORSE_BISTRO[] PROGMEM = "-... .. ... - .-. ---s";
const char _MORSE_ROBUST[] PROGMEM = ".-. --- -... ..- ... -s";
const char _MORSE_SAEBEL[] PROGMEM = "... .-.- -... . .-..s";
const char _MORSE_SUELZE[] PROGMEM = "... ..-- .-.. --.. .s";
const char _MORSE_SEKTOR[] PROGMEM = "... . -.- - --- .-.s";
const char _MORSE_VEKTOR[] PROGMEM = "...- . -.- - --- .-.s";
const char _MORSE_BRAVO[] PROGMEM = "-... .-. .- ...- ---s";
const char _MORSE_KOBRA[] PROGMEM = "-.- --- -... .-. .-s";
const char _MORSE_BOMBE[] PROGMEM = "-... --- -- -... .s";
const char _MORSE_SUEDEN[] PROGMEM = "... ..-- -.. . -.s";
const char *const MORSE_TABLE[] PROGMEM = {_MORSE_HALLE, _MORSE_HALLO, _MORSE_LOKAL, _MORSE_STEAK, _MORSE_STUECK, _MORSE_SPECK, _MORSE_BISTRO, _MORSE_ROBUST, _MORSE_SAEBEL, _MORSE_SUELZE, _MORSE_SEKTOR, _MORSE_VEKTOR, _MORSE_BRAVO, _MORSE_KOBRA, _MORSE_BOMBE, _MORSE_SUEDEN};

const char _MORSE_FREQUENCY_HALLE[] PROGMEM = "3505";
const char _MORSE_FREQUENCY_HALLO[] PROGMEM = "3515";
const char _MORSE_FREQUENCY_LOKAL[] PROGMEM = "3522";
const char _MORSE_FREQUENCY_STEAK[] PROGMEM = "3532";
const char _MORSE_FREQUENCY_STUECK[] PROGMEM = "3535";
const char _MORSE_FREQUENCY_SPECK[] PROGMEM = "3542";
const char _MORSE_FREQUENCY_BISTRO[] PROGMEM = "3545";
const char _MORSE_FREQUENCY_ROBUST[] PROGMEM = "3552";
const char _MORSE_FREQUENCY_SAEBEL[] PROGMEM = "3555";
const char _MORSE_FREQUENCY_SUELZE[] PROGMEM = "3565";
const char _MORSE_FREQUENCY_SEKTOR[] PROGMEM = "3572";
const char _MORSE_FREQUENCY_VEKTOR[] PROGMEM = "3575";
const char _MORSE_FREQUENCY_BRAVO[] PROGMEM = "3582";
const char _MORSE_FREQUENCY_KOBRA[] PROGMEM = "3592";
const char _MORSE_FREQUENCY_BOMBE[] PROGMEM = "3595";
const char _MORSE_FREQUENCY_SUEDEN[] PROGMEM = "3600";
const char *const MORSE_FREQUENCY_TABLE[] PROGMEM = {_MORSE_FREQUENCY_HALLE, _MORSE_FREQUENCY_HALLO, _MORSE_FREQUENCY_LOKAL, _MORSE_FREQUENCY_STEAK, _MORSE_FREQUENCY_STUECK, _MORSE_FREQUENCY_SPECK, _MORSE_FREQUENCY_BISTRO, _MORSE_FREQUENCY_ROBUST, _MORSE_FREQUENCY_SAEBEL, _MORSE_FREQUENCY_SUELZE, _MORSE_FREQUENCY_SEKTOR, _MORSE_FREQUENCY_VEKTOR, _MORSE_FREQUENCY_BRAVO, _MORSE_FREQUENCY_KOBRA, _MORSE_FREQUENCY_BOMBE, _MORSE_FREQUENCY_SUEDEN};

const int MORSE_BETWEEN_TIME = 300;
const int MORSE_DOT_TIME = 400;
const int MORSE_DASH_TIME = 1000;
const int MORSE_AFTER_LETTER_TIME = 1500;
const int MORSE_EOL_TIME = 6000;

int morseCodeIndex = 0;
char *morseWordPtr = nullptr;
int morseCodeStep = -1;
bool morseCallNext = false;
int morseFrequencyIndex = 8;
bool morseButtonWasPressed = false;

U8X8_SSD1309_128X64_NONAME2_HW_I2C yellowDisplay;

void setDisplayText()
{
  char phrase[8];
  strcpy_P(phrase, (char *)pgm_read_word(&(MORSE_FREQUENCY_TABLE[morseFrequencyIndex])));
  phrase[4] = ' ';
  phrase[5] = 'M';
  phrase[6] = 'H';
  phrase[7] = 'z';

  multiplexer.selectChannel(MULTIPLEXER_Morse);
  yellowDisplay.clearDisplay();
  for (byte i = 0; i < 8; i++)
  {
    yellowDisplay.drawGlyph(i * 2, 3, phrase[i]);
  }
}

void morseInitRead(byte index)
{
  morseCodeIndex = index;
  morseWordPtr = (char *)pgm_read_word(&(MORSE_TABLE[morseCodeIndex]));

  multiplexer.selectChannel(MULTIPLEXER_Morse);
  yellowDisplay.begin();
  yellowDisplay.setFont(u8x8_font_px437wyse700a_2x2_r);
}

void morseStart()
{
  morseCallNext = true;
  morseCodeStep = 0;

  setDisplayText();
}

void morseLogicLoop()
{
  if (activeModules[2] && !solvedModules[2] && morseCallNext && morseCodeStep != -1)
  {
    morseCallNext = false;
    handleMorseCodeStep();
  }
}

void morseLogicButtonLoop()
{
  if (digitalRead(INPUT_Morse_Left) && digitalRead(INPUT_Morse_Right) && digitalRead(INPUT_Morse_Send))
  {
    morseButtonWasPressed = false;
  }
  else if (!morseButtonWasPressed)
  {
    morseButtonWasPressed = true;
    engageLogicCooldown();

    if (!digitalRead(INPUT_Morse_Right))
    {
      if (morseFrequencyIndex > 0)
      {
        morseFrequencyIndex--;
        setDisplayText();
      }
    }
    else if (!digitalRead(INPUT_Morse_Left))
    {
      if (morseFrequencyIndex < 15)
      {
        morseFrequencyIndex++;
        setDisplayText();
      }
    }
    else if (!loopSerialWriteCooldown && !digitalRead(INPUT_Morse_Send))
    {
      sendSerialData(0x6, morseCodeIndex == morseFrequencyIndex ? 1 : 0);
    }
  }
}

void handleMorseCodeStep()
{
  char morseChar = pgm_read_byte_near(morseWordPtr + morseCodeStep);

  if (morseChar == 's')
  {
    morseCodeStep = 0;
    timer.in(MORSE_EOL_TIME, handleMorseNextLetter);
  }
  else if (morseChar == '.')
  {
    morseCodeStep++;
    digitalWrite(OUTPUT_Morse_LED, HIGH);
    timer.in(MORSE_DOT_TIME, handleMorseOff);
  }
  else if (morseChar == '-')
  {
    morseCodeStep++;
    digitalWrite(OUTPUT_Morse_LED, HIGH);
    timer.in(MORSE_DASH_TIME, handleMorseOff);
  }
  else if (morseChar == ' ')
  {
    morseCodeStep++;
    timer.in(MORSE_AFTER_LETTER_TIME, handleMorseNextLetter);
  }
}

bool handleMorseOff(void *)
{
  digitalWrite(OUTPUT_Morse_LED, LOW);
  timer.in(MORSE_BETWEEN_TIME, handleMorseNextLetter);
  return false; // to repeat the action - false to stop
}

bool handleMorseNextLetter(void *)
{
  morseCallNext = true;
  return false; // to repeat the action - false to stop
}

void morsePowerOff()
{
  multiplexer.selectChannel(MULTIPLEXER_Morse);
  yellowDisplay.begin();
  yellowDisplay.clearDisplay();
  yellowDisplay.setPowerSave(1);
  digitalWrite(OUTPUT_Morse_LED, LOW);
  morseCallNext = false;
  morseCodeStep = -1;
}