#include "common.h"
#include "modules/wires.h"

byte connectedWires = 0x0;

void wiresInit() {
  connectedWires = PINF & 0b00011111;
}

void wiresSerialWriteLoop() {
    byte newConnectedWires = PINF & 0b00011111;

    if(connectedWires != newConnectedWires) {
        connectedWires = newConnectedWires;

        Serial.write(0x2);
        Serial.write(connectedWires);     
        engageSerialWriteCooldown();
    }
}