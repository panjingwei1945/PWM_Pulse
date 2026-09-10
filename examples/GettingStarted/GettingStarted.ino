/*
  GettingStarted

  First example: compare constant output with and without ramp-down.
  Arduino Mega 2560: p1 = D6.
*/

#include <PWM_Pulse.h>

const uint32_t OUTPUT_DURATION_MS = 1000; // Output duration in ms.
const uint32_t RAMP_TIME_MS = 1000; // Ramp-down duration in ms, approximately.
const int POWER_PERCENT = 25; // Power setting in percent (0-100).

void setup() {
  PWM_PULSE.p1_init();
}

void loop() {
  // Basic constant output without ramp-down.
  PWM_PULSE.p1_constant(OUTPUT_DURATION_MS, 0, POWER_PERCENT);
  delay(OUTPUT_DURATION_MS); // Wait for the output to finish.
  delay(1000); // Separate the two demonstrations.

  // Constant output followed by ramp-down.
  PWM_PULSE.p1_constant_ramp(OUTPUT_DURATION_MS, 0, POWER_PERCENT, RAMP_TIME_MS);
  delay(OUTPUT_DURATION_MS + RAMP_TIME_MS); // Wait for output and ramp-down.
  delay(2000); // Pause before repeating.
}
