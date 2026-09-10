/*
  MultiplePulses

  Compare p1_multipulses() with its ramp-down variant.
  Arduino Mega 2560: p1 = D6.
*/
#include <PWM_Pulse.h>

const uint32_t DURATION_MS = 1000; // Pulse train duration in ms.
const float PULSE_FREQUENCY_HZ = 10.0; // Pulses per second.
const uint32_t PULSE_WIDTH_MS = 20; // ON time of each pulse in ms.
const uint32_t PRE_TRIGGER_DELAY_MS = 0; // Delay before output starts in ms.
const uint32_t RAMP_TIME_MS = 1000; // Ramp-down duration in ms, approximately.
// Power setting (0-100%), applied as PWM duty cycle; not calibrated optical power.
const int POWER_PERCENT = 50; // Power setting in percent (0-100).

void setup() { PWM_PULSE.p1_init(); }

void loop() {
  // Pulse train without ramp-down.
  PWM_PULSE.p1_multipulses(DURATION_MS, PULSE_FREQUENCY_HZ, PULSE_WIDTH_MS,
                           PRE_TRIGGER_DELAY_MS, POWER_PERCENT);
  delay(PRE_TRIGGER_DELAY_MS + DURATION_MS); // Wait for the pulse train to finish.
  delay(1000); // Separate the two demonstrations.

  // Pulse train followed by ramp-down.
  PWM_PULSE.p1_multipulses_ramp(DURATION_MS, PULSE_FREQUENCY_HZ, PULSE_WIDTH_MS,
                                PRE_TRIGGER_DELAY_MS, POWER_PERCENT, RAMP_TIME_MS);
  delay(PRE_TRIGGER_DELAY_MS + DURATION_MS + RAMP_TIME_MS); // Wait for output and ramp-down.
  delay(2000);
}
