/* CancelOutput: Arduino Mega 2560, p1 on D8. */
#include <PWM_Pulse.h>

const uint32_t OUTPUT_DURATION_MS = 3000;
const uint32_t CANCEL_AFTER_MS = 500;
const int PWM_SETTING = 40;

void setup() { PWM_PULSE.init(); }

void loop() {
  PWM_PULSE.p1_constant(OUTPUT_DURATION_MS, 0, PWM_SETTING);
  delay(CANCEL_AFTER_MS);
  PWM_PULSE.p1_cancel();
  // Allow interrupt-driven cancellation to complete before reusing p1.
  delay(100);
  delay(2000);
}
