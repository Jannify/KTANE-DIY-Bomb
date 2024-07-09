#include "common.h"
#include "modules/wires.h"

byte connectedWires = 0x0;

void wiresInit()
{
  return;
  connectedWires = PINF & 0b00011111;
}

void wiresSerialWriteLoop()
{
  return;
  byte newConnectedWires = PINF & 0b00011111;

  if (connectedWires != newConnectedWires)
  {
    byte cutWire = connectedWires ^ newConnectedWires;
    connectedWires = newConnectedWires;

    sendSerialData(0x2, cutWire << 3);
  }
}