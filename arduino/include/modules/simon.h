#pragma once

void simonInit(byte data0, byte data1);
void simonLogicLoop();
void simonSerialWriteLoop();
void simonPowerOff();

bool handleSimonSequenceOn(void *);
bool handleSimonSequenceOff(void *);
bool handleSimonAllOff(void *);
