#include "common.h"
#include "modules/morse.h"

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

const int MORSE_BETWEEN_TIME = 400;
const int MORSE_DOT_TIME = 500;
const int MORSE_DASH_TIME = 750;
const int MORSE_AFTER_LETTER_TIME = 1000;
const int MORSE_EOL_TIME = 5000;

int morseCodeIndex = 0;
char *morseWordPtr = nullptr;
int morseCodeStep = -1;
bool morseCallNext = false;
int morseFrequencyIndex = 8;
bool morseButtonWasPressed = false;

void morseInitRead()
{
  while (Serial.available() < 1)
  {
  }
  morseCodeIndex = Serial.read();
  morseWordPtr = (char *)pgm_read_word(&(MORSE_TABLE[morseCodeIndex]));
}

void morseLogicLoop()
{
  if (morseCallNext && morseCodeStep != -1)
  {
    morseCallNext = false;
    handleMorseCodeStep();
  }
}

void morseLogicButtonLoop()
{
  if (!digitalRead(INPUT_Morse_Left) && !digitalRead(INPUT_Morse_Right) && !digitalRead(INPUT_Morse_Send))
  {
    morseButtonWasPressed = false;
  }
  else if (!morseButtonWasPressed)
  {
    morseButtonWasPressed = true;
    engageLogicCooldown();

    if (digitalRead(INPUT_Morse_Left))
    {
      if (morseFrequencyIndex > 0)
      {
        morseFrequencyIndex--;
        char phrase[10];
        strcpy_P(phrase, (char *)pgm_read_word(&(MORSE_FREQUENCY_TABLE[morseFrequencyIndex])));
        strcat(phrase, " MHz");
        // TODO: Display output
      }
    }
    else if (digitalRead(INPUT_Morse_Right))
    {
      if (morseFrequencyIndex < 15)
      {
        morseFrequencyIndex++;
      }
    }
  }
}

void morseSerialWriteLoop()
{
  if (!morseButtonWasPressed && digitalRead(INPUT_Morse_Send))
  {
    morseButtonWasPressed = true; // Reset is in loopLogic()
    Serial.write(0x6);
    Serial.write(morseCodeIndex == morseFrequencyIndex);
    engageLogicCooldown();
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