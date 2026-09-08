/* PreTriggerDelay: Arduino Mega 2560, p1 on D8, marker on D43. */
#include <PWM_Pulse.h>

const uint8_t MARKER_PIN = 43;
const uint32_t PRE_TRIGGER_DELAY_MS = 100;
const uint32_t OUTPUT_DURATION_MS = 500;
const int PWM_SETTING = 50;

void setup() {
  pinMode(MARKER_PIN, OUTPUT);
  digitalWrite(MARKER_PIN, LOW);
  PWM_PULSE.init();
}

void loop() {
  digitalWrite(MARKER_PIN, HIGH);
  delay(1);
  digitalWrite(MARKER_PIN, LOW);
  PWM_PULSE.p1_constant(OUTPUT_DURATION_MS, PRE_TRIGGER_DELAY_MS, PWM_SETTING);
  delay(PRE_TRIGGER_DELAY_MS + OUTPUT_DURATION_MS + 20);
  delay(2000);
}
