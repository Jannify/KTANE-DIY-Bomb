#pragma once

void morseInitRead();
void morseLogicLoop();
void morseLogicButtonLoop();
void morseSerialWriteLoop();

void handleMorseCodeStep();
bool handleMorseOff(void *);
bool handleMorseNextLetter(void *);
