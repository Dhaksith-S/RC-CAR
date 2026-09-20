#include <Arduino.h>
#include "config.h"
#include "motor_control.h"
#include "wifi_control.h"
#include "http_server.h"

void setup() {
  Serial.begin(115200);
  delay(300);

  Serial.println();
  Serial.println("=== ESP32 RC Car Firmware ===");

  motorControlInit();
  Serial.println("Motor pins configured, motors OFF");

  wifiApInit();
  httpServerInit();

  Serial.println("Ready.");
}

void loop() {
  httpServerHandle();
  failsafeCheck();333 ṁ
}
