# PWM_Pulse

Three-channel PWM-modulated pulse output for **Arduino Mega 2560 (ATmega2560, 16 MHz)**, intended for controlling a laser driver's modulation input. Developed in Ninglong Xu's lab (xulab).

Current version: `v0.3.4`.

Repository: [panjingwei1945/PWM_Pulse](https://github.com/panjingwei1945/PWM_Pulse).

Author and maintainer: Jingwei Pan <jwpan@ion.ac.cn>.

## Hardware

| Channel | Arduino pin | PWM source | Timing source |
| --- | --- | --- | --- |
| p1 | D8 | Timer4 / OC4C | Timer1 |
| p2 | D7 | Timer4 / OC4B | Timer3 |
| p3 | D6 | Timer4 / OC4A | Timer5 |

Connect the chosen output to a compatible laser driver's modulation input and connect the grounds. These pins provide control signals, not laser drive current. Check the driver's input requirements before connecting it.

All channels share an approximately 19.98 kHz PWM carrier. Pulse timing uses 1 ms ticks. Do not independently reconfigure Timer4 or an initialized channel's timing timer. Unused channels' timing timers are left unchanged, but this library still defines the Timer1/3/5 compare-A interrupt handlers, so other libraries defining those handlers will conflict. The `avr` metadata denotes the processor architecture; it does **not** imply support for Uno, Nano or every AVR board.

## Installation

Copy the library folder into your Arduino sketchbook's `libraries` directory, or install a ZIP containing the library via **Sketch > Include Library > Add .ZIP Library**.

Select **Arduino Mega or Mega 2560** from the Arduino AVR Boards package. Open **File > Examples > PWM_Pulse** after installation.

| Example | Function |
| --- | --- |
| `MultiplePulses` | Generate repeated pulses. |
| `PreTriggerDelay` | Delay the start of output. |
| `CancelAnytime` | Stop output early, directly or with ramp-down. |
| `ThreeChannelOutput` | Run three output channels together. |

## Quick start

```cpp
#include <PWM_Pulse.h>

void setup() {
  PWM_PULSE.init();
}

void loop() {
  // D8: 10 Hz pulses, 20 ms ON, 50% power setting, for 500 ms.
  PWM_PULSE.p1_multipulses(500, 10.0f, 20, 0, 50);
  delay(2000);  // Let the task finish before reusing this channel.
}
```

Output tasks run in interrupts after the call returns. Application `delay()` calls do not provide the library's timing.

## API

Call `PWM_PULSE.init()` to initialize all channels, or initialize only the channels you use:

```cpp
PWM_PULSE.p1_init(); // D8 and Timer1.
PWM_PULSE.p2_init(); // D7 and Timer3; does not reset p1.
PWM_PULSE.p3_init(); // D6 and Timer5; does not reset p1 or p2.
```

Timer4 is configured once and shared by all channels. Repeated initialization does not reset active outputs. Per-channel initialization preserves the global interrupt state; interrupts must be enabled for pulse timing. The original `init()` enables global interrupts as before.

In the following table, replace `px` with `p1`, `p2` or `p3`.

| Method | Behavior |
| --- | --- |
| `px_multipulses(duration, fq, p_width, pre_trg_delay, power)` | Run a pulse train for the specified duration. |
| `px_constant(duration, pre_trg_delay, power)` | Maintain a constant power setting for the specified duration. |
| `px_multipulses_ramp(duration, fq, p_width, pre_trg_delay, power, ramp_step)` | Run a pulse train, then gradually reduce power to zero. |
| `px_constant_ramp(duration, pre_trg_delay, power, ramp_step)` | Maintain constant output, then gradually reduce power to zero. |
| `px_cancel()` | Request termination at the next timing interrupt. |
| `px_cancel_ramp(ramp_step)` | Stop output early by gradually reducing power to zero. |

| Parameter | Meaning |
| --- | --- |
| `duration` | Positive output duration in ms, excluding predelay and ramp time. |
| `fq` | Finite positive pulse frequency in Hz, distinct from the PWM carrier. |
| `p_width` | Positive ON duration per pulse in ms. For separate pulses, leave at least 1 ms of quantized OFF time. |
| `pre_trg_delay` | Delay before output in ms; zero starts immediately. |
| `power` | Power setting in percent (0–100), controlled by PWM duty cycle. |
| `ramp_step` | Ramp-down steps; each step takes approximately 1 ms. Use a positive value. |

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
