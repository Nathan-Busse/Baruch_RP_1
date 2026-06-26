/*
 * Baruch Quadruped – Servo Horn Calibration (Nathan-Busse Library)
 *
 * Uses the standard BaruchServo library.
 * Two PCA9685 boards:
 *   Board 1 (0x40) -> Right‑Front (0-2) & Left‑Rear (3-5)
 *   Board 2 (0x41) -> Left‑Front (0-2) & Right‑Rear (3-5)
 *
 * Serial commands:
 *   list               – show all servo names and current angles
 *   all 90             – set ALL servos to 90°
 *   RF_coxa 93         – set Right‑Front coxa to 93°
 *   LF_femur 88        – set Left‑Front femur to 88°
 *   ...
 */

#include <Wire.h>
#include <BaruchServo.h>

// ---------- USER CONFIGURATION ----------
#define SERVO_FREQ    50        // Hz
#define MIN_PULSE_US  500       // pulse width for   0°
#define MAX_PULSE_US  2500      // pulse width for 180°

// Two boards with different I²C addresses
BaruchServo pwm1 = BaruchServo(0x41);
BaruchServo pwm2 = BaruchServo(0x40);

// ---------- SERVO NAME -> (board pointer, channel) MAPPING ----------
struct ServoMapping {
  const char* name;
  BaruchServo* pwm;   // pointer to the board object
  uint8_t channel;                // 0‑15
};

ServoMapping servoMap[] = {
  // ===== Board 1 (0x40) =====
  {"RF_coxa",  &pwm1, 8},
  {"RF_femur", &pwm1, 7},
  {"RF_tibia", &pwm1, 6},

  {"LR_coxa",  &pwm1, 9},
  {"LR_femur", &pwm1, 10},
  {"LR_tibia", &pwm1, 11},

  // ===== Board 2 (0x41) =====
  {"LF_coxa",  &pwm2, 9},
  {"LF_femur", &pwm2, 10},
  {"LF_tibia", &pwm2, 11},

  {"RR_coxa",  &pwm2, 8},
  {"RR_femur", &pwm2, 7},
  {"RR_tibia", &pwm2, 6}
};

const uint8_t numServos = sizeof(servoMap) / sizeof(servoMap[0]);

// ============================
void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println(F("Initialising both PCA9685 boards…"));

  pwm1.begin();
  pwm1.setPWMFreq(SERVO_FREQ);
  pwm2.begin();
  pwm2.setPWMFreq(SERVO_FREQ);

  // Set all servos to 90° (centre)
  setAllServos(90);
  Serial.println(F("All servos set to 90°."));
  printHelp();
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    if (input.length() == 0) return;

    if (input == "list") { 
      
      printServoList();
    }
    else if (input.startsWith("all")) {
      float angle = 90.0;
      sscanf(input.c_str(), "all %f", &angle);
      setAllServos(angle);
      Serial.print(F("All servos set to "));
      Serial.print(angle);
      Serial.println(F("°"));
    }
    else {
      char name[20];
      float angle;
      if (sscanf(input.c_str(), "%s %f", name, &angle) == 2) {
        int idx = findServoIndex(name);
        if (idx >= 0) {
          setAngle(servoMap[idx].pwm, servoMap[idx].channel, angle);
          Serial.print(name);
          Serial.print(F(" set to "));
          Serial.print(angle);
          Serial.println(F("°"));
        } else {
          Serial.print(F("Unknown servo: "));
          Serial.println(name);
          printHelp();
        }
      } else {
        Serial.println(F("Bad command. Use 'name angle', 'all angle', or 'list'"));
      }
    }
  }
}

// ============================
// Helper: convert degrees (0‑180) to pulse length and send to a board
void setAngle(BaruchServo* pwm, uint8_t ch, float angle) {
  angle = constrain(angle, 0.0f, 180.0f);
  uint16_t pulse = map((long)angle, 0, 180, MIN_PULSE_US, MAX_PULSE_US);
  pwm->writeMicroseconds(ch, pulse);   // uses the built‑in convenience method
  // alternative: pwm->setPWM(ch, 0, pulseToTicks(pulse));
}

// Set all mapped servos to a given angle
void setAllServos(float angle) {
  for (uint8_t i = 0; i < numServos; i++) {
    setAngle(servoMap[i].pwm, servoMap[i].channel, angle);
  }
}

// Find a servo by name
int findServoIndex(const char* name) {
  for (uint8_t i = 0; i < numServos; i++) {
    if (strcmp(name, servoMap[i].name) == 0) return i;
  }
  return -1;
}

// Print current state of all servos
void printServoList() {
  Serial.println(F("Servo name\tBoard\tCh\tAngle"));
  for (uint8_t i = 0; i < numServos; i++) {
    // We don't have a direct "get current angle" method,
    // so we display the last target we set (this sketch keeps no state).
    // For calibration purposes that's fine.
    Serial.print(servoMap[i].name);
    Serial.print("\t\t");
    Serial.print(servoMap[i].pwm == &pwm1 ? "0x40" : "0x41");
    Serial.print("\t");
    Serial.print(servoMap[i].channel);
    Serial.println("\t(unknown)");
  }
}

void printHelp() {
  Serial.println(F("\nCommands:"));
  Serial.println(F("  list          - show all servos"));
  Serial.println(F("  all <angle>   - set all servos to angle"));
  Serial.println(F("  <name> <angle> - set specific servo"));
  Serial.println(F("Example: RF_coxa 93"));
  Serial.println(F("Servo names:"));
  for (uint8_t i = 0; i < numServos; i++) {
    Serial.print(F("  "));
    Serial.println(servoMap[i].name);
  }
}