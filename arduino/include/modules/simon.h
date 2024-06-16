#pragma once

void simonInit(byte, byte, byte);
void simonLogicLoop();
void simonSerialWriteLoop();
bool handleSimonSequenceOn(void *);
bool handleSimonSequenceOff(void *);
bool handleSimonAllOff(void *);