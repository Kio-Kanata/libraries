#include "BluetoothHIDMaster_mod.h"
#include "BluetoothPSController.h"

BluetoothHIDMaster hid;
BluetoothPSController dualSense;

void setup() {
  Serial.begin(115200);
  Serial.println("connecting...");
  resetConnection();
  dualSense.attachButtonHandler(BUTTON::CIRCLE, STATUS::PRESSED, circlePressedFunc);
  dualSense.attachButtonHandler(BUTTON::CIRCLE, STATUS::HELD, circleHeldFunc, 500);
  dualSense.attachButtonHandler(BUTTON::CIRCLE, STATUS::RELEASED, circleReleasedFunc);
}

void loop() {
  if (BOOTSEL) {
    Serial.println("resetting connection...");
    resetConnection();
    while (BOOTSEL) delay(1);
  }
  delay(100);
}

void resetConnection() {
  hid.onRawReport(onRaw);
  hid.begin();
  delay(300);
  hid.clearPairing();
  hid.connectAny();
}

void onRaw(void*, const uint8_t* rpt, uint16_t n) {
  dualSense.onRawReport(rpt, n, millis());
}

void circlePressedFunc() {
  Serial.println("pressed");
}

void circleHeldFunc() {
  Serial.println("held");
}

void circleReleasedFunc() {
  Serial.println("released");
}