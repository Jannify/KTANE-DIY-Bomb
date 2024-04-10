#include "common.h"
#include "modules/simon.h"

const int SIMON_ON_TIME = 500;
const int SIMON_OFF_TIME = 1000;
const int SIMON_BETWEEN_TIME = 5000;

int simonSequence[6];
int simonSequenceLength = 0;
int simonSequenceStep = -1;
bool simonSequenceCallNext = false;
bool simonReadWasPressed = false;
int simonAcceptButtonInput = false;

void simonInit(byte data0, byte data1, byte data2)
{
  simonSequenceLength = (data0 & 0b11100000) >> 5;
  simonSequence[0] = (data0 & 0b00001100) >> 2;
  simonSequence[1] = (data0 & 0b00000011);
  simonSequence[2] = (data1 & 0b11000000) >> 6;
  simonSequence[3] = (data1 & 0b00110000) >> 4;
  simonSequence[4] = (data1 & 0b00001100) >> 2;
  simonSequence[5] = (data1 & 0b00000011);

  simonSequenceStep = 0;
  simonSequenceCallNext = true;
  simonAcceptButtonInput = false;
}

void simonLogicLoop()
{
  if (simonSequenceCallNext)
  {
    simonSequenceCallNext = false;
    int ledPin = OUTPUT_Simon_Blue + simonSequence[simonSequenceStep];
    digitalWrite(ledPin, HIGH);
    timer.in(SIMON_ON_TIME, handleSimonSequenceOn, (void *)ledPin);
  }
}

void simonSerialWriteLoop()
{
  if (simonAcceptButtonInput)
  {
    int simonButtonPressed = -1;
    if (digitalRead(INPUT_Simon_Blue))
    {
      simonButtonPressed = 0;
    }
    else if (digitalRead(INPUT_Simon_Red))
    {
      simonButtonPressed = 1;
    }
    else if (digitalRead(INPUT_Simon_Yellow))
    {
      simonButtonPressed = 2;
    }
    else if (digitalRead(INPUT_Simon_Green))
    {
      simonButtonPressed = 3;
    }

    if (simonButtonPressed == -1)
    {
      simonReadWasPressed = false;
    }
    else if (!simonReadWasPressed)
    {
      simonReadWasPressed = true;



      // if (simonSequence[simonReadStep] == simonButtonPressed)
      // {
      //   simonReadStep++;

      //   if (simonReadStep >= simonSequenceLength)
      //   {
      //     Serial.write(0x4); // 3Bit Length + zero + 6 * (2 Bit Button index)
      //     Serial.write(simonButtonPressed);
      //     simonReadStep = -1;
      //   }
      // }
      // else
      // {
      //   Serial.write(0x4);
      //   Serial.write(0x0);
      //   simonReadStep = 0;
      // }


      Serial.write(0x4);
      Serial.write(simonButtonPressed);
      engageSerialWriteCooldown();

      int pin = OUTPUT_Simon_Blue + simonButtonPressed;
      digitalWrite(pin, HIGH);
      timer.in(SIMON_ON_TIME, setPinLow, (void *)pin);
    }
  }
}

bool handleSimonSequenceOn(void *argument)
{
  digitalWrite((int)argument, LOW);
  timer.in(SIMON_OFF_TIME, handleSimonSequenceOff, argument);
  return false; // to repeat the action - false to stop
}

bool handleSimonSequenceOff(void *)
{
  if (simonSequenceStep >= (simonSequenceLength - 1))
  {
    simonSequenceStep = -1;
    simonAcceptButtonInput = true;
    return false;
  }

  simonSequenceStep++;
  simonSequenceCallNext = true;
  return false; // to repeat the action - false to stop
}