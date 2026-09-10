/*
  MultiplePulses

  Use p1_multipulses() to generate repeated pulses with adjustable frequency, width and power.
  Arduino Mega 2560: p1 = D6.
*/
#include <PWM_Pulse.h>

const uint32_t DURATION_MS = 500; // Pulse train duration in ms.
const float PULSE_FREQUENCY_HZ = 10.0; // Pulses per second.
const uint32_t PULSE_WIDTH_MS = 20; // ON time of each pulse in ms.
const uint32_t PRE_TRIGGER_DELAY_MS = 0; // Delay before output starts in ms.
// Power setting (0-100%), applied as PWM duty cycle; not calibrated optical power.
const int POWER_PERCENT = 50; // Power setting in percent (0-100).

void setup() { PWM_PULSE.p1_init(); }

void loop() {
  PWM_PULSE.p1_multipulses(DURATION_MS, PULSE_FREQUENCY_HZ, PULSE_WIDTH_MS,
                           PRE_TRIGGER_DELAY_MS, POWER_PERCENT);
  delay(PRE_TRIGGER_DELAY_MS + DURATION_MS);
  delay(2000);
}
