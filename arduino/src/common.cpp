#include "common.h"

Timer<20> timer;

bool loopLogicButtonCooldown = false;
bool loopSerialWriteCooldown = false;

void engageLogicCooldown() {
  loopLogicButtonCooldown = true;
  timer.in(50, [](void *) -> bool {
    loopLogicButtonCooldown = false;
    return false;
  });
}

void engageSerialWriteCooldown() {
  loopSerialWriteCooldown = true;
  timer.in(50, [](void *) -> bool {
    loopSerialWriteCooldown = false;
    return false;
  });
}

bool setPinLow(void *argument) {
  digitalWrite((int)argument, LOW);
  return false;
}


void reset() {
  //Reset BigButton
  //Reset 7Bit
  //Reset I2C
  //Reset SPI
  timer.cancel();
  digitalWrite(OUTPUT_RESET, LOW);
}