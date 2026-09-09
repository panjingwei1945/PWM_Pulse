# PWM_Pulse

Three-channel PWM-modulated pulse output for **Arduino Mega 2560 (ATmega2560, 16 MHz)**, intended for controlling a laser driver's modulation input.

Current version: `v0.3.3`.

Repository: [panjingwei1945/PWM_Pulse](https://github.com/panjingwei1945/PWM_Pulse).

Author and maintainer: Jingwei Pan <jwpan@ion.ac.cn>.

## Hardware

| Channel | Arduino pin | PWM source | Timing source |
| --- | --- | --- | --- |
| p1 | D8 | Timer4 / OC4C | Timer1 |
| p2 | D7 | Timer4 / OC4B | Timer3 |
| p3 | D6 | Timer4 / OC4A | Timer5 |

Connect the chosen output to a compatible laser driver's modulation input and connect the grounds. These pins provide control signals, not laser drive current. Check the driver's input requirements before connecting it.

All channels share an approximately 19.98 kHz PWM carrier. Pulse timing uses 1 ms ticks. Do not independently reconfigure Timer1, Timer3, Timer4 or Timer5 from other code or libraries. The `avr` metadata denotes the processor architecture; it does **not** imply support for Uno, Nano or every AVR board.

## Installation

Until Library Manager registration is complete, put this library folder in your Arduino sketchbook's `libraries` directory, or import a ZIP containing the library via **Sketch > Include Library > Add .ZIP Library**.

Select **Arduino Mega or Mega 2560** from the Arduino AVR Boards package. Open **File > Examples > PWM_Pulse** after installation. The examples include `example_version001`, `BasicPulseTrain`, `PreTriggerDelay` and `CancelOutput`.

## Quick start

```cpp
#include <PWM_Pulse.h>

void setup() {
  PWM_PULSE.init();
}

void loop() {
  // D8: 10 Hz pulses, 20 ms ON, 50% PWM setting, for 500 ms.
  PWM_PULSE.p1_multipulses(500, 10.0f, 20, 0, 50);
  delay(2000);  // Let the task finish before reusing this channel.
}
```

Output tasks run in interrupts after the call returns. Application `delay()` calls do not provide the library's timing.

## API

Call `PWM_PULSE.init()` once before starting outputs. In the following table, replace `px` with `p1`, `p2` or `p3`.

| Method | Behavior |
| --- | --- |
| `px_multipulses(duration, fq, p_width, pre_trg_delay, power)` | Run a pulse train for the specified duration. |
| `px_constant(duration, pre_trg_delay, power)` | Maintain the PWM setting for the specified duration. |
| `px_multipulses_ramp(duration, fq, p_width, pre_trg_delay, power, ramp_step)` | Append a continuous ramp down after a pulse train. |
| `px_constant_ramp(duration, pre_trg_delay, power, ramp_step)` | Append a ramp down after constant output. |
| `px_cancel()` | Request termination at the next timing interrupt. |
| `px_cancel_ramp(ramp_step)` | Request early termination through ramp down. |

| Parameter | Meaning |
| --- | --- |
| `duration` | Positive output duration in ms, excluding delay and ramp time. |
| `fq` | Finite positive pulse frequency in Hz, distinct from the PWM carrier. |
| `p_width` | Positive ON duration per pulse in ms. For separate pulses, leave at least 1 ms of quantized OFF time. |
| `pre_trg_delay` | Delay before output in ms; zero starts immediately. |
| `power` | Setting from 0 to 100, mapped to PWM compare values from 0 to 800; not calibrated optical power. |
| `ramp_step` | Positive step count; one step per ms, giving approximately this many ms of ramp time. |

The included original example produces a 1 ms marker on D43, then runs three constant outputs for 500 ms at settings of 100, 60 and 20, followed by approximately 1000 ms of ramp down and a 2000 ms pause.

## Current limitations

- Parameter validation is incomplete. Callers must respect the documented ranges and avoid overflow in time calculations.
- Replacing an active task on the same channel does not fully reset its state. Wait for completion before starting another task. Returning from `cancel()` does not mean cleanup has completed.
- Cancellation normally takes up to approximately one timing tick; delayed interrupts delay it further.
- A ramp is appended to the whole output stage, not to each pulse. It uses stored power and may re-enable output when entered during an OFF phase or pre-trigger delay.
- OFF durations are truncated to whole milliseconds, so actual pulse frequency may differ from the requested frequency. Widths at or above the period produce continuous output.
- Sequential channel calls do not guarantee strictly simultaneous onset.
- Main-code and interrupt state updates currently lack full concurrency protection.

Release packaging does not change these existing behaviors. Hardware timing and optical output still require measurement on the actual setup.

## Development and release

See [RELEASING.md](RELEASING.md) for validation commands and the Library Manager submission steps.

## License

Released under the [MIT License](LICENSE).

Copyright (c) 2026 Jingwei Pan.
