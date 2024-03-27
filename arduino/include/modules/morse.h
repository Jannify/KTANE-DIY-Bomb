#pragma once

void morseInit(int);
void morseLogicLoop();
void morseLogicButtonLoop();
void morseSerialWriteLoop();

void handleMorseCodeStep();
bool handleMorseOff(void *);
bool handleMorseNextLetter(void *);