#include <Adafruit_NeoPixel.h>
#include <arduino-timer.h>

bool toggleLastTry(void *);
bool handleSimonSequenceOn(void *argument);
bool handleSimonSequenceOff(void *argument);

#define INPUT_WIRE_1 A0
#define INPUT_WIRE_2 A1
#define INPUT_WIRE_3 A2
#define INPUT_WIRE_4 A3
#define INPUT_WIRE_5 A4
#define INPUT_WIRE_6 A5
#define INPUT_BigButton A6
#define INPUT_Simon_Blue A7
#define INPUT_Simon_Red A8
#define INPUT_Simon_Yellow A9
#define INPUT_Simon_Green A10
#define INPUT_Memory_1 A11
#define INPUT_Memory_2 A12
#define INPUT_Memory_3 A13
#define INPUT_Memory_4 A14
#define INPUT_Morse_Send A15
#define INPUT_Morse_Left 37
#define INPUT_Morse_Right 38
#define INPUT_Pass_Send 39
#define INPUT_Pass_1_Up 40
#define INPUT_Pass_2_Up 41
#define INPUT_Pass_3_Up 42
#define INPUT_Pass_4_Up 43
#define INPUT_Pass_5_Up 44
#define INPUT_Pass_1_Down 45
#define INPUT_Pass_2_Down 46
#define INPUT_Pass_3_Down 47
#define INPUT_Pass_4_Down 48
#define INPUT_Pass_5_Down 49

#define OUTPUT_Indikator 0
#define OUTPUT_Morse_LED 1
#define OUTPUT_BigButton_Color 2
#define OUTPUT_BigButton_Strip 3
#define OUTPUT_7Bit_Data 4
#define OUTPUT_7Bit_Clock_0 5
#define OUTPUT_7Bit_Clock_1 6
#define OUTPUT_7Bit_Clock_2 7
#define OUTPUT_7Bit_Clock_3 8
#define OUTPUT_7Bit_Clock_4 9
#define OUTPUT_Memory_Lvl_1 10
#define OUTPUT_Memory_Lvl_2 11
#define OUTPUT_Memory_Lvl_3 12
#define OUTPUT_Memory_Lvl_4 13
#define OUTPUT_Simon_Blue 14  // Needs to be in succession
#define OUTPUT_Simon_Red 15
#define OUTPUT_Simon_Yellow 16
#define OUTPUT_Simon_Green 17
#define OUTPUT_Tries_1 18
#define OUTPUT_Tries_2 19
#define OUTPUT_Module_0 22
#define OUTPUT_Module_1 23
#define OUTPUT_Module_2 24
#define OUTPUT_Module_3 25
#define OUTPUT_Module_4 26
#define OUTPUT_Module_5 27
#define OUTPUT_Module_6 28
#define OUTPUT_Module_7 29
#define OUTPUT_Module_8 30
#define OUTPUT_Module_9 31
#define OUTPUT_Module_10 32
#define OUTPUT_RESET 33

#define I2C_SDA 20
#define I2C_SCL 21
#define SPI_CIPO 50
#define SPI_COPI 51
#define SPI_SCK 52
#define SPI_SS 53

const char *const MORSE_TABLE[] PROGMEM = {
  ".... .- .-.. .-.. .s",     // halle
  ".... .- .-.. .-.. ---s",   // hallo
  ".-.. --- -.- .- .-..s",    // lokal
  "... - . .- -.-s",          // steak
  "... - ..-- -.-. -.-s",     // stück
  "... .--. . -.-. -.-s",     // speck
  "-... .. ... - .-. ---s",   // bistro
  ".-. --- -... ..- ... -s",  // robust
  "... .-.- -... . .-..s",    // säbel
  "... ..-- .-.. --.. .s",    // sülze
  "... . -.- - --- .-.s",     // sektor
  "...- . -.- - --- .-.s",    // vektor
  "-... .-. .- ...- ---s",    // bravo
  "-.- --- -... .-. .-s",     // kobra
  "-... --- -- -... .s",      // bombe
  "... ..-- -.. . -.s"        // süden
};

const int SIMON_ON_TIME = 500;
const int SIMON_OFF_TIME = 1000;
const int SIMON_BETWEEN_TIME = 5000;

const int MORSE_DOT_TIME = 500;
const int MORSE_DASH_TIME = 750;
const int MORSE_AFTER_LETTER_TIME = 5000;
const int MORSE_EOL_TIME = 10000;

auto timer = timer_create_default();

Adafruit_NeoPixel bigKnob = Adafruit_NeoPixel(7, OUTPUT_BigButton_Color, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel bigKnobStrip = Adafruit_NeoPixel(1, OUTPUT_BigButton_Strip, NEO_GRB + NEO_KHZ800);


void setup() {
  digitalWrite(OUTPUT_RESET, HIGH);
  for (int i = 0; i <= 33; i++) {
    pinMode(i, OUTPUT);
  }
  for (int i = 37; i <= 49; i++) {
    pinMode(i, INPUT);
  }

  Serial.begin(9600);
}

bool bombStarted = false;
byte minutes = 0;
byte seconds = 0;  //TODO: Implement timer
bool lastTry = false;
Timer<>::Task lastTryTask = nullptr;

int simonSequence[10];
int simonSequenceLength = 0;
int simonSequenceStep = -1;
bool simonSequenceCallNext = false;

int morseCodeIndex = 0;
int morseCodeStep = -1;  //TODO: Impplement loop
bool morseCallNext = false;


void loop() {
  timer.tick();

  if (simonSequenceCallNext) {
    simonSequenceCallNext = false;
    int ledPin = OUTPUT_Simon_Blue + simonSequence[simonSequenceStep];
    digitalWrite(ledPin, HIGH);
    timer.in(SIMON_ON_TIME, handleSimonSequenceOn, (void *)ledPin);
  }


  if (Serial.available()) {
    int type = Serial.read();
    switch (type) {
      case 0:  // Reset
        reset();
        break;
      case 1:  // Init
        {
          while (Serial.available() < 6) {}
          byte serialNumber[6];
          Serial.readBytes(serialNumber, 6);
          //TODO: Set serialNumber to screen

          while (Serial.available() < 1) {}
          if ((bool)Serial.read()) {
            digitalWrite(OUTPUT_Indikator, HIGH);
          } else {
            digitalWrite(OUTPUT_Indikator, LOW);
          }

          while (Serial.available() < 1) {}
          morseCodeIndex = Serial.read();

          while (Serial.available() < 3) {}
          byte indicatorLetters[3];
          Serial.readBytes(indicatorLetters, 3);

          while (Serial.available() < 3) {}
          byte bigKnobColor[3];
          Serial.readBytes(bigKnobColor, 3);
          for (int i = 0; i <= 7; i++) {
            bigKnob.setPixelColor(i, bigKnobColor[0], bigKnobColor[1], bigKnobColor[2]);
          }
          bigKnob.show();

          while (Serial.available() < 1) {}
          byte bigKnobTextLength = Serial.read();
          while (Serial.available() < bigKnobTextLength) {}
          byte bigKnobText[10];
          Serial.readBytes(bigKnobText, bigKnobTextLength);
          //TODO: Set bigKnobText to screen
          break;
        }
      case 2:  // Start
        {
          while (Serial.available() < 2) {}
          minutes = Serial.read();
          seconds = Serial.read();
          bombStarted = true;
          break;
        }
      case 3:  // Set Tries
        {
          if (lastTryTask != nullptr) {
            timer.cancel(lastTryTask);
            lastTryTask = nullptr;
          }
          while (Serial.available() < 1) {}
          byte tries = Serial.read();
          if (tries >= 2) {
            digitalWrite(OUTPUT_Tries_1, HIGH);
            digitalWrite(OUTPUT_Tries_2, HIGH);
            lastTryTask = timer.every(600, toggleLastTry);
          } else if (tries >= 1) {
            digitalWrite(OUTPUT_Tries_1, HIGH);
            digitalWrite(OUTPUT_Tries_2, LOW);
          } else {
            digitalWrite(OUTPUT_Tries_1, LOW);
            digitalWrite(OUTPUT_Tries_2, LOW);
          }
          break;
        }
      case 4:  // Set Solved
        {
          while (Serial.available() < 2) {}
          byte solvedModules1 = Serial.read();
          byte solvedModules2 = Serial.read();

          PORTA = solvedModules2;

          digitalWrite(OUTPUT_Module_8, LOW);
          digitalWrite(OUTPUT_Module_9, LOW);
          digitalWrite(OUTPUT_Module_10, LOW);
          if (solvedModules1 & B00000100) {
            digitalWrite(OUTPUT_Module_10, HIGH);
          } else if (solvedModules1 & B00000010) {
            digitalWrite(OUTPUT_Module_9, HIGH);
          } else if (solvedModules1 & B00000001) {
            digitalWrite(OUTPUT_Module_8, HIGH);
          }
          break;
        }
      case 5:  // BigKnob Strip
        {
          while (Serial.available() < 3) {}
          byte bigKnobStripColor[3];
          Serial.readBytes(bigKnobStripColor, 3);
          for (int i = 0; i <= 1; i++) {
            bigKnobStrip.setPixelColor(i, bigKnobStripColor[0], bigKnobStripColor[1], bigKnobStripColor[2]);
          }
          bigKnobStrip.show();
          break;
        }
      case 6:  // BigKnob Strip
        {
          while (Serial.available() < 5) {}
          byte passwordLetters[5];
          Serial.readBytes(passwordLetters, 5);
          //TODO: Set passwordLetters to screen
          break;
        }
      case 7:  // SimonSays
        {
          while (Serial.available() < 3) {}
          byte data[3];
          Serial.readBytes(data, 3);

          simonSequenceLength = (data[0] & 0b11110000) >> 4;
          simonSequence[0] = (data[0] & 0b00001100) >> 2;
          simonSequence[1] = (data[0] & 0b00000011);
          simonSequence[2] = (data[1] & 0b11000000) >> 6;
          simonSequence[3] = (data[1] & 0b00110000) >> 4;
          simonSequence[4] = (data[1] & 0b00001100) >> 2;
          simonSequence[5] = (data[1] & 0b00000011);
          simonSequence[6] = (data[2] & 0b11000000) >> 6;
          simonSequence[7] = (data[2] & 0b00110000) >> 4;
          simonSequence[8] = (data[2] & 0b00001100) >> 2;
          simonSequence[9] = (data[2] & 0b00000011);

          simonSequenceStep = 0;
          simonSequenceCallNext = true;
          break;
        }
      case 8:  // Memory
        {
          while (Serial.available() < 2) {}
          byte data[2];
          Serial.readBytes(data, 2);

          int bigNumber = (data[0] & 0b11000000) >> 6;
          int smallNumber1 = (data[0] & 0b00110000) >> 4;
          int smallNumber2 = (data[0] & 0b00001100) >> 2;
          int smallNumber3 = (data[0] & 0b00000011);
          int smallNumber4 = (data[1] & 0b11000000) >> 6;
          int level = (data[1] & 0b00111000) >> 3;

          digitalWrite(OUTPUT_Memory_Lvl_1, LOW);
          digitalWrite(OUTPUT_Memory_Lvl_2, LOW);
          digitalWrite(OUTPUT_Memory_Lvl_3, LOW);
          digitalWrite(OUTPUT_Memory_Lvl_4, LOW);
          if (level >= 4) {
            digitalWrite(OUTPUT_Memory_Lvl_4, HIGH);
          }
          if (level >= 3) {
            digitalWrite(OUTPUT_Memory_Lvl_3, HIGH);
          }
          if (level >= 2) {
            digitalWrite(OUTPUT_Memory_Lvl_2, HIGH);
          }
          if (level >= 1) {
            digitalWrite(OUTPUT_Memory_Lvl_1, HIGH);
          }
          break;
        }
    }
  }
}

bool toggleLastTry(void *) {
  digitalWrite(OUTPUT_Tries_1, !digitalRead(OUTPUT_Tries_1));
  digitalWrite(OUTPUT_Tries_2, !digitalRead(OUTPUT_Tries_2));
  return true;
}

bool handleSimonSequenceOn(void *argument) {
  digitalWrite((int)argument, LOW);
  timer.in(SIMON_OFF_TIME, handleSimonSequenceOff, argument);
  return false;  // to repeat the action - false to stop
}

bool handleSimonSequenceOff(void *) {
  if (simonSequenceStep >= (simonSequenceLength - 1)) {
    simonSequenceStep = -1;
    return false;
  }

  simonSequenceStep++;
  simonSequenceCallNext = true;
  return false;  // to repeat the action - false to stop
}

bool handleMorseOn(void *argument) {
  digitalWrite((int)argument, LOW);
  timer.in(SIMON_OFF_TIME, handleSimonSequenceOff, argument);
  return false;  // to repeat the action - false to stop
}

bool handleMorseOff(void *) {
  if (simonSequenceStep >= (simonSequenceLength - 1)) {
    simonSequenceStep = -1;
    return false;
  }

  simonSequenceStep++;
  simonSequenceCallNext = true;
  return false;  // to repeat the action - false to stop
}

void reset() {
  //Reset BigButton
  //Reset 7Bit
  //Reset I2C
  //Reset SPI
  timer.cancel();
  digitalWrite(OUTPUT_RESET, LOW);
}
