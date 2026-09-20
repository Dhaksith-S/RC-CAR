#pragma once
#include <IPAddress.h>

// ======================= Wi-Fi Access Point =======================
// Phone connects directly to this network. No internet required/used.
#define WIFI_AP_SSID      "RC-Car"
#define WIFI_AP_PASSWORD  "rccar1234"          // WPA2 requires >= 8 characters

static const IPAddress WIFI_AP_IP(192, 168, 4, 1);       // ESP32's own address
static const IPAddress WIFI_AP_GATEWAY(192, 168, 4, 1);
static const IPAddress WIFI_AP_SUBNET(255, 255, 255, 0);

// ======================= L298N pin mapping =======================
// OUT1+OUT2 = left motor, driven by IN1/IN2 + ENA (PWM)
// OUT3+OUT4 = right motor, driven by IN3/IN4 + ENB (PWM)
#define PIN_ENA   25   // Left motor speed (PWM)
#define PIN_IN1   26   // Left motor direction bit A
#define PIN_IN2   27   // Left motor direction bit B
#define PIN_IN3   14   // Right motor direction bit A
#define PIN_IN4   13   // Right motor direction bit B
#define PIN_ENB   33   // Right motor speed (PWM)

// ======================= PWM (LEDC) =======================
// ESP32 Arduino core 3.x API: ledcAttach(pin, freq, resolution) / ledcWrite(pin, duty).
// 8-bit resolution gives a 0-255 duty range, matching the speed range used everywhere
// else in this firmware and in the HTTP API, so no scaling is needed.
#define PWM_FREQ_HZ          5000
#define PWM_RESOLUTION_BITS  8

// ======================= Speed limits =======================
#define SPEED_MIN          0
#define SPEED_MAX          255
// Safety cap applied to every incoming command while we're still bringing the car up.
// Raise this later once basic movement is confirmed safe.
#define SPEED_DEFAULT_MAX  200

// ======================= Failsafe =======================
// If no valid /move command arrives within this window while the car is moving,
// motors are stopped automatically. Covers app crashes, Wi-Fi drops, etc.
#define COMMAND_TIMEOUT_MS 800

// ======================= HTTP =======================
#define HTTP_PORT 80
