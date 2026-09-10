/*
  CancelAnytime

  Demonstrate flexible control of the output duration during runtime.
  Compare immediate cancellation with cancellation followed by ramp-down.
  This example uses a fixed delay to simulate a runtime stop decision.
  Arduino Mega 2560: p1 = D6.
*/
#include <PWM_Pulse.h>

const uint32_t OUTPUT_DURATION_MS = 3000; // Automatic stop time in ms if not canceled earlier.
const uint32_t CANCEL_AFTER_MS = 500; // Simulated stop decision after 500 ms; shorter than the output limit.
const uint32_t RAMP_TIME_MS = 1000; // Ramp-down duration in ms, approximately.
// Power setting (0-100%), applied as PWM duty cycle; not calibrated optical power.
const int POWER_PERCENT = 40; // Power setting in percent (0-100).

void setup() { PWM_PULSE.p1_init(); }

void loop() {
  // Direct cancellation.
  PWM_PULSE.p1_constant(OUTPUT_DURATION_MS, 0, POWER_PERCENT);
  delay(CANCEL_AFTER_MS); // In an application, cancel when a runtime condition is met.
  PWM_PULSE.p1_cancel(); // Request shutdown on the next timer interrupt.
  delay(1); // Wait for the next timer interrupt to apply cancellation.
  delay(2000); // Separate the two cancellation demonstrations.

  // Cancellation with a gradual power decrease.
  PWM_PULSE.p1_constant(OUTPUT_DURATION_MS, 0, POWER_PERCENT);
  delay(CANCEL_AFTER_MS);
  PWM_PULSE.p1_cancel_ramp(RAMP_TIME_MS); // Start ramp-down instead of switching off directly.
  delay(RAMP_TIME_MS); // Allow ramp-down to finish.
  delay(2000);
}
