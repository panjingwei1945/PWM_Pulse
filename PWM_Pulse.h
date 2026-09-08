/* Version 03
 * create in 20141119
 * Modified in 20260908
 * This library generates 3 parallel PWM-modulated pulse outputs on ATmega2560.
 *

1.	px_sqr_wave(int pin, unsigned long duration, float fq, unsigned long p_width, int mode)

	This function return immediately and generate serial pulse in background. 

	pin: Indicate the pin you want to generate pulses.

	duratuon: Unit is ms. It indicate lasting time of the generated serial pusles.

	fq: Unit is Hz .You should notice that it means fq pusles per second(not a ms).

	p_width: Unit is ms. It indicate the period of phase "ON" which setted by variable "mode".

	mode: Must set this variable to one of two constants:"TRG_L" and "TRG_H". 
			TRG_L indicate the phase "ON" to be a LOW voltage. And phase "OFF" to be HIGH. 
			TRG_H indicate the phase "ON" to be a HIGH voltage. And phase "OFF" to be LOW. 


2.	px_pulse(int pin, uint32_t duration, int mode);

	this function return immediately and generate one pulse in background. 

	pin: Indicate the pin you want to generate pulses.

	duratuon: Unit is ms. It indicate lasting time of the generated serial pusles.

	mode: Must set this variable to one of two constants:"TRG_L" and "TRG_H". 
			TRG_L indicate the phase "ON" to be a LOW voltage. And phase "OFF" to be HIGH. 
			TRG_H indicate the phase "ON" to be a HIGH voltage. And phase "OFF" to be LOW. 

3.	void px_cancel(); 

	This function return immediately. 
	It aims to shorten p1_sqr_wave and p1_pulse when you change you mind. 

---------Confliction with FAST_PWM-------
 * Caution: There are some confilcts when using both "FAST_PWM" and "Pulse".

 * For ATmega2560 usage (Timer4 PWM outputs):
 
 	channel	pin	PWM source	timing source
 	p1		8	OC4C		Timer1
 	p2		7	OC4B		Timer3
 	p3		6	OC4A		Timer5
 
 */


#ifndef _PWM_PULSE_h
#define _PWM_PULSE_h
#include "Arduino.h"

#define TRG_L 0
#define TRG_H 1 


class PWM_PULSE_Class
{
private:

public:

// // #if defined __AVR_ATmega328P__
// 	void init();
// 	void p1_sqr_wave(int pin, unsigned long duration, float fq, unsigned long p_width, int mode); 
// 	void p1_sqr_wave(int pin, unsigned long duration, float fq, unsigned long p_width, int mode, unsigned long pre_trg_delay); 
// 	void p1_pulse(int pin, uint32_t duration, int mode); 
// 	void p1_cancel(); 

// #elif defined __AVR_ATmega2560__
	void init();
	//void p1_sqr_wave(int pin, unsigned long duration, float fq, unsigned long p_width, int mode); 
	//void p1_sqr_wave(int pin, unsigned long duration, float fq, unsigned long p_width, int mode, unsigned long pre_trg_delay); 
	//void p1_pulse(int pin, uint32_t duration, int mode); 
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
