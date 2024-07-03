#pragma once

void morseInitRead();
void morseStart();
void morseLogicLoop();
void morseLogicButtonLoop();
void morseSerialWriteLoop();

void handleMorseCodeStep();
bool handleMorseOff(void *);
bool handleMorseNextLetter(void *);
