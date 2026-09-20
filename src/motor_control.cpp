#include "motor_control.h"
#include "config.h"

static bool motorsMoving = false;

static int clampSpeed(int value) {
  if (value > SPEED_MAX) return SPEED_MAX;
  if (value < -SPEED_MAX) return -SPEED_MAX;
  return value;
}

// Drives one motor: sets direction pins, writes PWM duty on its enable pin.
static void setSingleMotor(int speed, int pinIn1, int pinIn2, int pinEnable) {
  int clamped = clampSpeed(speed);

  if (clamped > 0) {
    digitalWrite(pinIn1, HIGH);
    digitalWrite(pinIn2, LOW);
  } else if (clamped < 0) {
    digitalWrite(pinIn1, LOW);
    digitalWrite(pinIn2, HIGH);
  } else {
    digitalWrite(pinIn1, LOW);
    digitalWrite(pinIn2, LOW);
  }

  // Core 3.x pin-based LEDC API: no manual channel assignment needed.
  ledcWrite(pinEnable, abs(clamped));
}

void motorControlInit() {
  pinMode(PIN_IN1, OUTPUT);
  pinMode(PIN_IN2, OUTPUT);
  pinMode(PIN_IN3, OUTPUT);
  pinMode(PIN_IN4, OUTPUT);

  // Motors OFF before PWM or anything else is configured.
  digitalWrite(PIN_IN1, LOW);
  digitalWrite(PIN_IN2, LOW);
  digitalWrite(PIN_IN3, LOW);
  digitalWrite(PIN_IN4, LOW);

  // Attach PWM to the enable pins (core 3.x auto-assigns the hardware channel).
  ledcAttach(PIN_ENA, PWM_FREQ_HZ, PWM_RESOLUTION_BITS);
  ledcAttach(PIN_ENB, PWM_FREQ_HZ, PWM_RESOLUTION_BITS);

  ledcWrite(PIN_ENA, 0);
  ledcWrite(PIN_ENB, 0);

  motorsMoving = false;
}

void driveDifferential(int leftSpeed, int rightSpeed) {
  setSingleMotor(leftSpeed, PIN_IN1, PIN_IN2, PIN_ENA);
  setSingleMotor(rightSpeed, PIN_IN3, PIN_IN4, PIN_ENB);
  motorsMoving = (leftSpeed != 0) || (rightSpeed != 0);
}

void stopMotors() {
  driveDifferential(0, 0);
}

bool motorsAreMoving() {
  return motorsMoving;
}
