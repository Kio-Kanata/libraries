#include "BluetoothHIDMaster_mod.h"
#include "BluetoothPSController.h"

BluetoothHIDMaster hid;
BluetoothPSController dualSense;

void setup() {
  Serial.begin(115200);
  Serial.println("connecting...");
  resetConnection();
  dualSense.setDeadzone(BUTTON::LS, DIRECTION::X, 0.03);
  dualSense.setDeadzone(BUTTON::LS, DIRECTION::Y, 0.03);
  dualSense.setDeadzone(BUTTON::RS, DIRECTION::X, 0.03);
  dualSense.setDeadzone(BUTTON::RS, DIRECTION::Y, 0.03);
  Serial.println("Print values of L2, R2, and sticks.");
  delay(5000);
}

void loop() {
  if (BOOTSEL) {
    Serial.println("resetting connection...");
    resetConnection();
    while (BOOTSEL) delay(1);
  }
  printData();
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

void printData() {
  Serial.print("L2 : ");
  Serial.print(dualSense.getValue(BUTTON::L2));
  Serial.print(", R2 : ");
  Serial.print(dualSense.getValue(BUTTON::R2));
  Serial.print(", stickL : ");
  Serial.print(dualSense.getValue(BUTTON::LS, DIRECTION::X));
  Serial.print(" , ");
  Serial.print(dualSense.getValue(BUTTON::LS, DIRECTION::Y));
  Serial.print(", stickR : ");
  Serial.print(dualSense.getValue(BUTTON::RS, DIRECTION::X));
  Serial.print(" , ");
  Serial.print(dualSense.getValue(BUTTON::RS, DIRECTION::Y));
  
  Serial.println("");
}