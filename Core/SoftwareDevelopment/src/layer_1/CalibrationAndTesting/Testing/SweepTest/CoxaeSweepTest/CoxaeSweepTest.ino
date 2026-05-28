/*
 * Baruch Quadruped – Sequence Demo: 0 → 90 → 180 → 90 → 0 (per servo)
 *
 * For each servo:
 *   1. Smooth move to 0°, pause
 *   2. Smooth move to 90°, pause
 *   3. Smooth move to 180°, pause
 *   4. Smooth move back to 90°, pause
 *   5. Smooth move back to 0°, pause
 *   6. Wait INTER_SERVO_PAUSE, then next servo
 *
 * All delays are easily adjustable below.
 */

#include <Wire.h>
#include <BaruchServo.h>

// ---------- USER SETTINGS ----------
#define SERVO_FREQ          50      // Hz
#define MIN_PULSE_US        500     // 0°
#define MAX_PULSE_US        2500    // 180°

#define SWEEP_STEP_DELAY    15      // ms per 2° step – smaller = faster movement
#define PAUSE_AT_TARGET     800     // ms pause at each target angle (0, 90, 180, 90, 0)
#define INTER_SERVO_PAUSE   2000    // ms after the full sequence, before next servo
#define CYCLE_PAUSE         5000    // ms after all 12 servos, before repeating

// Two PCA9685 boards
BaruchServo pwm1 = BaruchServo(0x41);   // Board 1 (0x40 in your mapping)
BaruchServo pwm2 = BaruchServo(0x40);   // Board 2 (0x41)

struct ServoMapping {
  const char* name;
  BaruchServo* pwm;
  uint8_t channel;
};

ServoMapping servoMap[] = {
  // Board 1 (0x40)
  {"RF_coxa",  &pwm1, 0},
  {"RF_femur", &pwm1, 1},
  {"RF_tibia", &pwm1, 2},
  {"LR_coxa",  &pwm1, 13},
  {"LR_femur", &pwm1, 14},
  {"LR_tibia", &pwm1, 15},

  // Board 2 (0x41)
  {"LF_coxa",  &pwm2, 13},
  {"LF_femur", &pwm2, 14},
  {"LF_tibia", &pwm2, 15},
  {"RR_coxa",  &pwm2, 0},
  {"RR_femur", &pwm2, 1},
  {"RR_tibia", &pwm2, 2}
};

const uint8_t numServos = sizeof(servoMap) / sizeof(servoMap[0]);

// Keep track of the current angle of every servo (for smooth moves)
float currentAngle[numServos];

// ---------- HELPER: set raw angle ----------
void setAngle(BaruchServo* pwm, uint8_t ch, float angle) {
  angle = constrain(angle, 0.0f, 180.0f);
  uint16_t pulse = map((long)angle, 0, 180, MIN_PULSE_US, MAX_PULSE_US);
  pwm->writeMicroseconds(ch, pulse);
}

// ---------- Smooth move from currentAngle[idx] to target, with steps ----------
void smoothMoveTo(uint8_t idx, float target) {
  BaruchServo* pwm = servoMap[idx].pwm;
  uint8_t ch = servoMap[idx].channel;
  float start = currentAngle[idx];
  if (start == target) return;   // already there

  int step = (target > start) ? 2 : -2;
  for (float a = start; (step > 0 ? a <= target : a >= target); a += step) {
    setAngle(pwm, ch, a);
    currentAngle[idx] = a;
    delay(SWEEP_STEP_DELAY);
  }
  // Ensure exact target at the end
  setAngle(pwm, ch, target);
  currentAngle[idx] = target;
}

// ---------- Demo one servo: sequence 0→90→180→90→0 with pauses ----------
void demoSingleServo(uint8_t idx) {
  Serial.print(F("Testing "));
  Serial.print(servoMap[idx].name);
  Serial.println(F(" ..."));

  // Sequence of target angles (will be reached in order)
  float targets[] = {0.0, 90.0, 180.0, 90.0, 0.0};
  for (uint8_t t = 0; t < 5; t++) {
    smoothMoveTo(idx, targets[t]);
    Serial.print(F("   -> reached "));
    Serial.print(targets[t], 0);
    Serial.println(F("°, pausing"));
    delay(PAUSE_AT_TARGET);
  }

  Serial.println(F("  done."));
  delay(INTER_SERVO_PAUSE);
}

void demoAllServos() {
  for (uint8_t i = 0; i < numServos; i++) {
    demoSingleServo(i);
  }
}

// ---------- SETUP & LOOP ----------
void setup() {
  Serial.begin(115200);
  pwm1.begin();
  pwm1.setPWMFreq(SERVO_FREQ);
  pwm2.begin();
  pwm2.setPWMFreq(SERVO_FREQ);

  // Centre all servos and initialise currentAngle[] to 90°
  for (uint8_t i = 0; i < numServos; i++) {
    setAngle(servoMap[i].pwm, servoMap[i].channel, 90);
    currentAngle[i] = 90.0;
  }
  delay(1000);

  Serial.println(F("Auto-demo started: 0 → 90 → 180 → 90 → 0 per servo."));
}

void loop() {
  demoAllServos();
  Serial.println(F("===== Full cycle complete. Pausing before next cycle... ====="));
  delay(CYCLE_PAUSE);
}