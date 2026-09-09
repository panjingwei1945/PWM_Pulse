/*
  CancelAnytime

  Use p1_cancel() to stop laser output whenever needed during program execution.
  Arduino Mega 2560: p1 = D8.
*/
#include <PWM_Pulse.h>

const uint32_t OUTPUT_DURATION_MS = 3000; // Output duration in ms if not canceled.
const uint32_t CANCEL_AFTER_MS = 500; // Wait time in ms before requesting cancellation.
// Power setting (0-100%), applied as PWM duty cycle; not calibrated optical power.
const int POWER_PERCENT = 40; // Power setting in percent (0-100).

void setup() { PWM_PULSE.init(); }

void loop() {
  PWM_PULSE.p1_constant(OUTPUT_DURATION_MS, 0, POWER_PERCENT);
  delay(CANCEL_AFTER_MS);
  PWM_PULSE.p1_cancel();
  // Allow interrupt-driven cancellation to complete before reusing p1.
  delay(100);
  delay(2000);
}
