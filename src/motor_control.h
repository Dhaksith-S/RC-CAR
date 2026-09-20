#pragma once
#include <Arduino.h>

// Call once from setup(), before Wi-Fi/HTTP init. Leaves motors OFF.
void motorControlInit();

// leftSpeed / rightSpeed: -255..255. Sign = direction, 0 = stopped.
// This is the single entry point for actually driving the motors, so that
// future joystick/differential-drive control only needs to call this —
// forward/backward/left/right are just specific (left, right) pairs.
void driveDifferential(int leftSpeed, int rightSpeed);

void stopMotors();
bool motorsAreMoving();
