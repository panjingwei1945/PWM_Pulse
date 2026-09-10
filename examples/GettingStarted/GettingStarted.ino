/*
  GettingStarted

  First example: generate a basic constant output with p1_constant().
  Arduino Mega 2560: p1 = D6.
*/

#include <PWM_Pulse.h>

const uint32_t OUTPUT_DURATION_MS = 500; // Output duration in ms.
const int POWER_PERCENT = 25; // Power setting in percent (0-100).

void setup() {
  PWM_PULSE.p1_init();
}

void loop() {
  PWM_PULSE.p1_constant(OUTPUT_DURATION_MS, 0, POWER_PERCENT);
  delay(OUTPUT_DURATION_MS);
  delay(2000); // Pause before repeating.
}
