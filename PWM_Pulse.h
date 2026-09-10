/* Version v0.4.1
 * create in 20141119
 * Modified in 20260908
 * Three PWM-modulated pulse outputs on ATmega2560, configured for 16 MHz.
 *
 * Fixed channel assignments:
 *   Channel   Arduino pin   PWM source       Timing source
 *   p1        D6            Timer4 / OC4A    Timer5
 *   p2        D7            Timer4 / OC4B    Timer3
 *   p3        D8            Timer4 / OC4C    Timer1
 * All channels share an approximately 19.98 kHz PWM carrier. Each channel
 * has separate power and pulse timing settings, with a 1 ms timing tick.
 * Other code must not reconfigure Timer4 or an initialized channel's timer. These APIs
 * do not accept arbitrary output pins or a trigger polarity argument.
 *
 * Call PWM_PULSE.init() for all channels, or px_init() for each used channel.
 * Below, px means p1, p2
 * or p3. Output tasks run in interrupts after the API call returns.
 *
 * APIs:
 *   init()
 *     Initialize all channels and enable global interrupts.
 *   px_init()
 *     Initialize one channel and its timing timer; preserve interrupt state.
 *     Shared Timer4 is configured once. Repeated initialization is a no-op
 *     for initialized channels, so active outputs are not reset.
 *     Timer1/3/5 interrupt vectors remain defined even for unused channels.
 *   px_multipulses(duration, fq, p_width, pre_trg_delay, power)
 *     After the delay, generate pulses for duration ms. ON uses the set
 *     PWM power; OFF writes a PWM compare value of zero. The final pulse
 *     may be cut short when duration expires.
 *   px_constant(duration, pre_trg_delay, power)
 *     After the delay, maintain the set PWM power for duration ms.
 *   px_multipulses_ramp(duration, fq, p_width, pre_trg_delay, power, ramp_step)
 *   px_constant_ramp(duration, pre_trg_delay, power, ramp_step)
 *     Append a continuous ramp down to zero after the output duration.
 *     This is not a ramp on every pulse and does not include a ramp up.
 *     The ramp stage does not retain the pulse train's ON/OFF timing.
 *   px_cancel()
 *     Request termination. Output is closed and state cleared at the
 *     next timing interrupt, normally within about 1 ms; delayed
 *     interrupts also delay cancellation.
 *   px_cancel_ramp(ramp_step)
 *     Request early termination through a ramp down in subsequent ticks.
 *
 * Parameters and caller requirements:
 *   duration       Output duration in ms, greater than zero. Excludes
 *                  pre-trigger delay and ramp time; not cumulative ON time.
 *   fq             Pulse frequency in Hz, finite and positive. This is
 *                  not the PWM carrier frequency. Period = 1000 / fq ms;
 *                  OFF time is truncated to whole ms, affecting frequency.
 *   p_width        ON time per pulse in ms, greater than zero. For separate
 *                  pulses, the quantized OFF time must be at least 1 ms.
 *                  Width >= period currently produces continuous output.
 *   pre_trg_delay  Delay before output in ms; zero starts immediately.
 *   power          Use 0 through 100. PWM compare value = power * 8;
 *                  this is not calibrated optical laser power.
 *   ramp_step      Positive ramp step count, one step per 1 ms tick.
 *                  Ramp time is approximately ramp_step ms, subject to
 *                  quantization and floating-point arithmetic.
 *
 * Current implementation limitations:
 *   Validation is incomplete; callers must respect parameter ranges and
 *   avoid overflow in time calculations.
 *   Starting an active channel does not fully reset its timing/ramp state.
 *   Wait for completion before reuse; cancel() returning does not mean
 *   that interrupt-driven cleanup has completed.
 *   Ramping uses the stored power and may re-enable output if entered
 *   during an OFF phase or pre-trigger delay. Sequential channel starts
 *   do not guarantee strictly simultaneous onset.
 */


#ifndef _PWM_PULSE_h
#define _PWM_PULSE_h
#include "Arduino.h"

// Legacy constants; current PWM APIs do not use a polarity argument.
#define TRG_L 0
#define TRG_H 1 


class PWM_PULSE_Class
{
private:

public:
	void init(); // Initialize all channels; enable global interrupts.
	void p1_init(); // Initialize D6 and Timer5 once; preserve interrupt state.
	void p2_init(); // Initialize D7 and Timer3 once; preserve interrupt state.
	void p3_init(); // Initialize D8 and Timer1 once; preserve interrupt state.
	void p1_multipulses(unsigned long duration, float fq, unsigned long p_width, unsigned long pre_trg_delay, int power);
	void p1_constant( uint32_t duration, unsigned long pre_trg_delay, int power);
	void p1_constant_ramp( uint32_t duration, unsigned long pre_trg_delay, int power,int ramp_step);
	void p1_multipulses_ramp(unsigned long duration, float fq, unsigned long p_width, unsigned long pre_trg_delay, int power,int ramp_step);
	void p1_cancel(); 
	void p1_cancel_ramp(int ramp_step); 

	void p2_multipulses(unsigned long duration, float fq, unsigned long p_width, unsigned long pre_trg_delay, int power);
	void p2_constant( uint32_t duration, unsigned long pre_trg_delay, int power);
	void p2_constant_ramp( uint32_t duration, unsigned long pre_trg_delay, int power,int ramp_step);
	void p2_multipulses_ramp(unsigned long duration, float fq, unsigned long p_width, unsigned long pre_trg_delay, int power,int ramp_step);
	void p2_cancel(); 
	void p2_cancel_ramp(int ramp_step); 

	void p3_multipulses(unsigned long duration, float fq, unsigned long p_width, unsigned long pre_trg_delay, int power);
	void p3_constant( uint32_t duration, unsigned long pre_trg_delay, int power);
	void p3_constant_ramp( uint32_t duration, unsigned long pre_trg_delay, int power, int ramp_step);
	void p3_multipulses_ramp(unsigned long duration, float fq, unsigned long p_width, unsigned long pre_trg_delay, int power,int ramp_step);
	void p3_cancel(); 
	void p3_cancel_ramp(int ramp_step); 
			
};

extern PWM_PULSE_Class PWM_PULSE;

#endif
