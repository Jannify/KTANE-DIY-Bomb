#pragma once

//Test
extern int morseFrequencyIndex;
void setDisplayText();

void morseInitRead(byte index);
void morseStart();
void morseLogicLoop();
void morseLogicButtonLoop();
void morsePowerOff();

void handleMorseCodeStep();
bool handleMorseOff(void *);
bool handleMorseNextLetter(void *);
