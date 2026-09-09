/*
  ThreeChannelOutput

  Run three output channels together, each with its own power setting.
  Arduino Mega 2560: p1 = D8, p2 = D7, p3 = D6; D43 marks the request sequence.
*/

#include <PWM_Pulse.h>

const uint8_t MARKER_PIN = 43; // Digital pin for the request marker.
const uint32_t OUTPUT_DURATION_MS = 500; // Constant output duration in ms before ramp-down.
const uint32_t PRE_TRIGGER_DELAY_MS = 0; // Delay before output starts in ms.
const uint32_t RAMP_TIME_MS = 1000; // Ramp-down steps at 1 ms per step; approximate duration in ms.
const uint32_t REST_INTERVAL_MS = 2000; // Rest time in ms after output and ramp-down.
const int POWER_P1 = 100; // Channel p1 power setting in percent (0-100).
const int POWER_P2 = 60; // Channel p2 power setting in percent (0-100).
const int POWER_P3 = 20; // Channel p3 power setting in percent (0-100).

void setup() {
  Serial.begin(115200);
  pinMode(MARKER_PIN, OUTPUT);
  digitalWrite(MARKER_PIN, LOW);
  // PWM_PULSE.p1_init();
  // PWM_PULSE.p2_init();
  // PWM_PULSE.p3_init();
  PWM_PULSE.init(); // Initialize all three channels.
}

void loop() {
  digitalWrite(MARKER_PIN, HIGH);
  delay(1);
  digitalWrite(MARKER_PIN, LOW);

  PWM_PULSE.p1_constant_ramp(OUTPUT_DURATION_MS, PRE_TRIGGER_DELAY_MS, POWER_P1, RAMP_TIME_MS);
  PWM_PULSE.p2_constant_ramp(OUTPUT_DURATION_MS, PRE_TRIGGER_DELAY_MS, POWER_P2, RAMP_TIME_MS);
  PWM_PULSE.p3_constant_ramp(OUTPUT_DURATION_MS, PRE_TRIGGER_DELAY_MS, POWER_P3, RAMP_TIME_MS);

  // Include delay, output time and approximate ramp time before channel reuse.
  delay(PRE_TRIGGER_DELAY_MS + OUTPUT_DURATION_MS + RAMP_TIME_MS);
  delay(REST_INTERVAL_MS);
}
