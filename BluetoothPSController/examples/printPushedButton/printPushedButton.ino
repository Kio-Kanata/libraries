#include "BluetoothHIDMaster_mod.h"
#include "BluetoothPSController.h"

BluetoothHIDMaster hid;
BluetoothPSController dualSense;

void setup() {
  Serial.begin(115200);
  Serial.println("connecting...");
  resetConnection();
  Serial.println("Print pushed button name.");
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
  if (dualSense.isPressed(BUTTON::UP)) Serial.print("up, ");
  if (dualSense.isPressed(BUTTON::RIGHT)) Serial.print("right, ");
  if (dualSense.isPressed(BUTTON::DOWN)) Serial.print("down, ");
  if (dualSense.isPressed(BUTTON::LEFT)) Serial.print("left, ");
  if (dualSense.isPressed(BUTTON::TRIANGLE)) Serial.print("triangle, ");
  if (dualSense.isPressed(BUTTON::CIRCLE)) Serial.print("circle, ");
  if (dualSense.isPressed(BUTTON::X)) Serial.print("x, ");
  if (dualSense.isPressed(BUTTON::SQUARE)) Serial.print("square, ");
  if (dualSense.isPressed(BUTTON::L1)) Serial.print("L1, ");
  if (dualSense.isPressed(BUTTON::R1)) Serial.print("R1, ");
  if (dualSense.isPressed(BUTTON::L2)) Serial.print("L2, ");
  if (dualSense.isPressed(BUTTON::R2)) Serial.print("R2, ");
  if (dualSense.isPressed(BUTTON::L3)) Serial.print("L3, ");
  if (dualSense.isPressed(BUTTON::R3)) Serial.print("R3, ");
  if (dualSense.isPressed(BUTTON::LS, DIRECTION::UP)) Serial.print("LS_up, ");
  if (dualSense.isPressed(BUTTON::LS, DIRECTION::RIGHT)) Serial.print("LS_right, ");
  if (dualSense.isPressed(BUTTON::LS, DIRECTION::DOWN)) Serial.print("LS_down, ");
  if (dualSense.isPressed(BUTTON::LS, DIRECTION::LEFT)) Serial.print("LS_left, ");
  if (dualSense.isPressed(BUTTON::RS, DIRECTION::UP)) Serial.print("RS_up, ");
  if (dualSense.isPressed(BUTTON::RS, DIRECTION::RIGHT)) Serial.print("RS_right, ");
  if (dualSense.isPressed(BUTTON::RS, DIRECTION::DOWN)) Serial.print("RS_down, ");
  if (dualSense.isPressed(BUTTON::RS, DIRECTION::LEFT)) Serial.print("RS_left, ");

  Serial.println("");
}