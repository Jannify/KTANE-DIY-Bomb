#pragma once

void morseInitRead(byte index);
void morseStart();
void morseLogicLoop();
void morseLogicButtonLoop();
void morsePowerOff();

void handleMorseCodeStep();
bool handleMorseOff(void *);
bool handleMorseNextLetter(void *);
