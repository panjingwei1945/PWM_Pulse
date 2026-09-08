/* BasicPulseTrain: Arduino Mega 2560, p1 on D8. */
#include <PWM_Pulse.h>

const uint32_t DURATION_MS = 500;
const float PULSE_FREQUENCY_HZ = 10.0f;
const uint32_t PULSE_WIDTH_MS = 20;
const uint32_t PRE_TRIGGER_DELAY_MS = 0;
const int PWM_SETTING = 50;

void setup() { PWM_PULSE.init(); }

void loop() {
  // 10 Hz pulses, 20 ms ON, PWM setting 50, for 500 ms.
  PWM_PULSE.p1_multipulses(DURATION_MS, PULSE_FREQUENCY_HZ, PULSE_WIDTH_MS,
                           PRE_TRIGGER_DELAY_MS, PWM_SETTING);
  delay(PRE_TRIGGER_DELAY_MS + DURATION_MS + 20);
  delay(2000);
}
