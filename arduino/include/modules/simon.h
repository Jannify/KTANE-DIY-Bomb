#pragma once

void simonInit(byte, byte, byte);
void simonLogicLoop();
void simonSerialWriteLoop();
bool handleSimonSequenceOn(void *argument);
bool handleSimonSequenceOff(void *);