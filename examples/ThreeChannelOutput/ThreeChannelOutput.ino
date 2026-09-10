/*
  ThreeChannelOutput

  Activate three output channels in sequence using different pre-trigger delays,
  then compare operation with and without ramp-down.
  Arduino Mega 2560: p1 = D6, p2 = D7, p3 = D8; D43 marks the request sequence.
*/

#include <PWM_Pulse.h>

const uint8_t MARKER_PIN = 43; // Digital pin for the request marker.
const uint32_t OUTPUT_DURATION_MS = 2000; // Constant output duration in ms before ramp-down.
const uint32_t PRE_TRIGGER_DELAY_P1_MS = 0; // p1 delay before output starts in ms.
const uint32_t PRE_TRIGGER_DELAY_P2_MS = 500; // p2 delay before output starts in ms.
const uint32_t PRE_TRIGGER_DELAY_P3_MS = 1000; // p3 delay before output starts in ms.
const uint32_t RAMP_TIME_MS = 1000; // Ramp-down steps at 1 ms per step; approximate duration in ms.
const uint32_t REST_INTERVAL_MS = 2000; // Rest time in ms after output and ramp-down.
const int POWER_P1 = 100; // Channel p1 power setting in percent (0-100).
const int POWER_P2 = 60; // Channel p2 power setting in percent (0-100).
const int POWER_P3 = 20; // Channel p3 power setting in percent (0-100).

void setup() {
  Serial.begin(115200);
  pinMode(MARKER_PIN, OUTPUT);
  digitalWrite(MARKER_PIN, LOW);
  PWM_PULSE.p1_init();
  PWM_PULSE.p2_init();
  PWM_PULSE.p3_init();
  // PWM_PULSE.init(); // Initialize all three channels.
}

void loop() {
  digitalWrite(MARKER_PIN, HIGH);
  delay(1); // Mark the output request on D43.
  digitalWrite(MARKER_PIN, LOW);

  // Three-channel constant output without ramp-down.
  PWM_PULSE.p1_constant(OUTPUT_DURATION_MS, PRE_TRIGGER_DELAY_P1_MS, POWER_P1);
  PWM_PULSE.p2_constant(OUTPUT_DURATION_MS, PRE_TRIGGER_DELAY_P2_MS, POWER_P2);
  PWM_PULSE.p3_constant(OUTPUT_DURATION_MS, PRE_TRIGGER_DELAY_P3_MS, POWER_P3);

  delay(PRE_TRIGGER_DELAY_P3_MS + OUTPUT_DURATION_MS); // Wait for all channels.
  delay(REST_INTERVAL_MS); // Separate the two demonstrations.

  // Three-channel constant output followed by ramp-down.
  PWM_PULSE.p1_constant_ramp(OUTPUT_DURATION_MS, PRE_TRIGGER_DELAY_P1_MS, POWER_P1, RAMP_TIME_MS);
  PWM_PULSE.p2_constant_ramp(OUTPUT_DURATION_MS, PRE_TRIGGER_DELAY_P2_MS, POWER_P2, RAMP_TIME_MS);
  PWM_PULSE.p3_constant_ramp(OUTPUT_DURATION_MS, PRE_TRIGGER_DELAY_P3_MS, POWER_P3, RAMP_TIME_MS);

  // Include delay, output time and approximate ramp time before channel reuse.
  delay(PRE_TRIGGER_DELAY_P3_MS + OUTPUT_DURATION_MS + RAMP_TIME_MS); // Wait for output and ramp-down.
  delay(REST_INTERVAL_MS);
}
