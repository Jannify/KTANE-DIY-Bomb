#pragma once

void simonInit(byte, byte, byte);
void simonLogicLoop();
void simonSerialWriteLoop();
void simonPowerOff();

bool handleSimonSequenceOn(void *);
bool handleSimonSequenceOff(void *);
bool handleSimonAllOff(void *);
