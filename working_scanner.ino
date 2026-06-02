#include <ESP32Servo.h>
#include <Wire.h>
#include <VL53L0X.h>

Servo panServo;
Servo tiltServo;
VL53L0X sensor;

#define PAN_PIN  5
#define TILT_PIN 4

// --- Justera till dina användbara ändlägen ---
const int PAN_MIN  = 20;
const int PAN_MAX  = 160;
const int TILT_MIN = 95;    // pekar uppåt (minskande tilt = upp)
const int TILT_MAX = 160;   // pekar nedåt
const int STEP     = 4;     // grovt först; sänk till 2 för finare scan

const int SETTLE_MS = 60;   // väntetid efter litet steg

void setup() {
  Serial.begin(115200);
  delay(500);

  Wire.begin();
  sensor.setTimeout(500);
  if (!sensor.init()) {
    Serial.println("Sensor init failed!");
    while (1);
  }
  // OBS: ingen startContinuous() — vi använder single-shot för
  // färska mätningar exakt vid varje position.

  panServo.attach(PAN_PIN);
  tiltServo.attach(TILT_PIN);

  panServo.write(PAN_MIN);
  tiltServo.write(TILT_MIN);
  delay(600);
}

void loop() {
  Serial.println("SCAN_START");
  Serial.println("pan,tilt,dist_mm");

  for (int pan = PAN_MIN; pan <= PAN_MAX; pan += STEP) {
    panServo.write(pan);
    tiltServo.write(TILT_MIN);   // återställ tilt till toppen
    delay(300);                  // större paus när både pan + tilt hoppar

    for (int tilt = TILT_MIN; tilt <= TILT_MAX; tilt += STEP) {
      tiltServo.write(tilt);
      delay(SETTLE_MS);

      uint16_t d = sensor.readRangeSingleMillimeters();  // färsk mätning
      if (sensor.timeoutOccurred() || d == 0 || d > 2000) continue;

      Serial.print(pan);  Serial.print(",");
      Serial.print(tilt); Serial.print(",");
      Serial.println(d);
    }
  }

  Serial.println("SCAN_END");

  // Parkera och stanna — gör bara en scan
  panServo.write(90);
  tiltServo.write(130);
  while (1) delay(1000);
}
