/*
  CancelAnytime

  Stop output early with p1_cancel(), or fade it out with p1_cancel_ramp().
  This example uses a fixed delay to simulate a runtime stop decision.
  Arduino Mega 2560: p1 = D8.
*/
#include <PWM_Pulse.h>

const uint32_t OUTPUT_DURATION_MS = 3000; // Automatic stop time in ms if not canceled earlier.
const uint32_t CANCEL_AFTER_MS = 500; // Simulated stop decision after 500 ms; shorter than the output limit.
const int RAMP_STEPS = 100; // Ramp-down steps, approximately 1 ms per step.
// Power setting (0-100%), applied as PWM duty cycle; not calibrated optical power.
const int POWER_PERCENT = 40; // Power setting in percent (0-100).

void setup() { PWM_PULSE.init(); }

void loop() {
  // Direct cancellation.
  PWM_PULSE.p1_constant(OUTPUT_DURATION_MS, 0, POWER_PERCENT);
  delay(CANCEL_AFTER_MS); // In an application, cancel when a runtime condition is met.
  PWM_PULSE.p1_cancel(); // Request shutdown on the next timer interrupt.
  // delay(1); // Optional. Cancellation normally takes up to 1 ms (next timer interrupt).
  delay(2000);

  // Cancellation with a gradual power decrease.
  PWM_PULSE.p1_constant(OUTPUT_DURATION_MS, 0, POWER_PERCENT);
  delay(CANCEL_AFTER_MS);
  PWM_PULSE.p1_cancel_ramp(RAMP_STEPS); // Start ramp-down instead of switching off directly.
  delay(RAMP_STEPS + 1); // Allow ramp-down to finish before the rest interval.
  delay(2000);
}
