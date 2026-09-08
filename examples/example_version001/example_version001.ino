/*
  ThreeChannelRamp

  Arduino Mega 2560 only.
  p1 = D8 (OC4C), p2 = D7 (OC4B), p3 = D6 (OC4A).
  D43 is a 1 ms request marker, not the measured laser-on edge.
  The channels start sequentially and are not guaranteed to be simultaneous.
  PWM settings are control values from 0 to 100, not optical power readings.
*/

#include <PWM_Pulse.h>

const uint8_t MARKER_PIN = 43;
const uint32_t OUTPUT_DURATION_MS = 500;
const uint32_t PRE_TRIGGER_DELAY_MS = 0;
const uint32_t RAMP_TIME_MS = 1000;
const uint32_t REST_INTERVAL_MS = 2000;
const int POWER_P1 = 100;
const int POWER_P2 = 60;
const int POWER_P3 = 20;

void setup() {
  Serial.begin(115200);
  pinMode(MARKER_PIN, OUTPUT);
  digitalWrite(MARKER_PIN, LOW);
  PWM_PULSE.init();
}

void loop() {
  digitalWrite(MARKER_PIN, HIGH);
  delay(1);
  digitalWrite(MARKER_PIN, LOW);

  PWM_PULSE.p1_constant_ramp(OUTPUT_DURATION_MS, PRE_TRIGGER_DELAY_MS, POWER_P1, RAMP_TIME_MS);
  PWM_PULSE.p2_constant_ramp(OUTPUT_DURATION_MS, PRE_TRIGGER_DELAY_MS, POWER_P2, RAMP_TIME_MS);
  PWM_PULSE.p3_constant_ramp(OUTPUT_DURATION_MS, PRE_TRIGGER_DELAY_MS, POWER_P3, RAMP_TIME_MS);

  // Include delay, output time and approximate ramp time before channel reuse.
  delay(PRE_TRIGGER_DELAY_MS + OUTPUT_DURATION_MS + RAMP_TIME_MS + 20);
  delay(REST_INTERVAL_MS);
}
