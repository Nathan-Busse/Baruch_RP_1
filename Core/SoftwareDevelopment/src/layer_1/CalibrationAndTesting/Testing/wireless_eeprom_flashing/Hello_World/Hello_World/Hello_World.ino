// Timing variables
unsigned long previousMillis = 0;
const unsigned long interval = 2000;  // 2 seconds

// Track if it's the first launch message
bool helloPrinted = false;

void setup() {
  Serial.begin(115200);
}

void loop() {
  unsigned long currentMillis = millis();

  // Print "Hello, world!" only once at startup
  if (!helloPrinted && Serial) {
    Serial.println("Hello, world! Wireless flash successful.");
    helloPrinted = true;
  }

  // Repeat system status message every 2 seconds
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    Serial.println("System active... Hello from Baruch!");
  }
}
