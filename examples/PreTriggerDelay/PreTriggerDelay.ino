/*
  PreTriggerDelay

  Start a constant output after a configurable delay.
  Arduino Mega 2560: p1 = D8; D43 marks the request sequence.
*/
#include <PWM_Pulse.h>

const uint8_t MARKER_PIN = 43; // Digital pin for the request marker.
const uint32_t PRE_TRIGGER_DELAY_MS = 100; // Delay before output starts in ms.
const uint32_t OUTPUT_DURATION_MS = 500; // Output duration in ms, excluding the delay.
// Power setting (0-100%), applied as PWM duty cycle; not calibrated optical power.
const int POWER_PERCENT = 50; // Power setting in percent (0-100).

void setup() {
  pinMode(MARKER_PIN, OUTPUT);
  digitalWrite(MARKER_PIN, LOW);
  PWM_PULSE.p1_init();
}

void loop() {
  digitalWrite(MARKER_PIN, HIGH);
  delay(1);
  digitalWrite(MARKER_PIN, LOW);
  PWM_PULSE.p1_constant(OUTPUT_DURATION_MS, PRE_TRIGGER_DELAY_MS, POWER_PERCENT);
  delay(PRE_TRIGGER_DELAY_MS + OUTPUT_DURATION_MS);
  delay(2000);
}
