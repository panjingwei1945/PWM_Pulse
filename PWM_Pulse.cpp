/* Version v0.4.2
* create in 20141119
* Modified in 20260908
* This library generates 3 parallel PWM-modulated pulse outputs on ATmega2560.
* Timer4 generates PWM on D6/D7/D8; Timer5/3/1 control p1/p2/p3 timing.
*/
#include "PWM_Pulse.h"
#include <util/atomic.h>

#define PRE_SCALE_64DIV (_BV(CS50) | _BV(CS51)) //4us
#define CTC_ISI 249 // CTC period: (249 + 1) * 4us = 1ms
#define CTC_FACTOR (1000/4/CTC_ISI)
#define PRE_SCALE_MASK (_BV(CS50) | _BV(CS51) | _BV(CS52))
#define PWM_PERIOD 800
#define PWM_LASER_DELAY 0 //us
#define PWM_PERIOD_OFFSET (PWM_LASER_DELAY * 16)

#define TRG_ON 1
#define TRG_OFF 0
//#define RAMPDOWN_STEPNUM 50
#define _SET(var,bit) var |= _BV(bit)
#define _CLEAR(var,bit) var &= ~_BV(bit)
uint8_t t_TCCRnA;
uint8_t t_TCCRnB;
uint8_t t_TCCRnC;
/* PWM pin definition on ATmega2560:
*	pin		PWM source		physical pin	operate interruptor
*	6		OC4A			PH3				p1 (Timer5)
*	7		OC4B			PH4				p2 (Timer3)
*	8		OC4C			PH5				p3 (Timer1)
*
*/

/***********t4_pwm***************/
/***********t4_pwm***************/
inline static void t4_PWM_init() {
	static bool initialized = false;
	if (initialized) return;
	TCCR4B = 0; // Stop the shared timer while configuring it once.

	t_TCCRnA = TCCR4A;
	_CLEAR(t_TCCRnA, WGM40); _SET(t_TCCRnA, WGM41); // Fast PWM, TOP: ICR4.
	// Connect only the outputs explicitly initialized by the caller.
	t_TCCRnA &= ~(_BV(COM4A1) | _BV(COM4A0) | _BV(COM4B1) |
	                _BV(COM4B0) | _BV(COM4C1) | _BV(COM4C0));

	t_TCCRnB = TCCR4B;
	_SET(t_TCCRnB, CS40); _CLEAR(t_TCCRnB, CS41); _CLEAR(t_TCCRnB, CS42); // No prescaling.
	_SET(t_TCCRnB, WGM42); _SET(t_TCCRnB, WGM43); // Fast PWM, TOP: ICR4.

	ICR4H = (PWM_PERIOD + PWM_PERIOD_OFFSET) >> 8;
	ICR4L = (uint8_t)(PWM_PERIOD + PWM_PERIOD_OFFSET); // PWM period is approximately 50us.

	OCR4AH = 0;
	OCR4AL = 0;
	OCR4BH = 0;
	OCR4BL = 0;
	OCR4CH = 0;
	OCR4CL = 0;

	TCNT4H = 0;
	TCNT4L = 0;

	TCCR4A = t_TCCRnA;
	TCCR4B = t_TCCRnB;
	TCCR4C = 0;
	initialized = true;
}

void PWM_write4A(uint16_t data)
{
	data = data + PWM_PERIOD_OFFSET;
	OCR4AH = data >> 8;
	OCR4AL = data;
}


void PWM_write4B(uint16_t data)
{
	data = data + PWM_PERIOD_OFFSET;
	OCR4BH = data >> 8;
	OCR4BL = data;
}


void PWM_write4C(uint16_t data)
{
	data = data + PWM_PERIOD_OFFSET;
	OCR4CH = data >> 8;
	OCR4CL = data;
}


float ramp_cal(float data, float rampdown_stepnum) //data can't be greater than 800. rampdown_stepnum
{
	return (data / rampdown_stepnum);
}

/***********p1***************/
/***********p1***************/
unsigned long t5_ON_DUR;
unsigned long t5_OFF_DUR;
unsigned long t5_PHASE_COUT_NUM;
unsigned long t5_DUR_COUT_NUM;
unsigned long t5_TIMEPASSED;
unsigned long t5_CUR_STATUS;
unsigned long t5_FLIPTIME;
unsigned long t5_PRETRG_DELAY_COUT_NUM;
float t5_REAL_PW;
float t5_POWER_RAMPSTEP;
unsigned long t5_RAMPCOUNTER;
int t5_RAMPSTEPNUM;
char t5_RAMPDOWN_FLAG;


inline static void _set_t5_isi(uint16_t isi){
	OCR5AH = isi >> 8;
	OCR5AL = isi;
}

inline static void t5_init(){
	t_TCCRnB = TCCR5B;
	_CLEAR(t_TCCRnB, CS50); _CLEAR(t_TCCRnB, CS51); _CLEAR(t_TCCRnB, CS52); //Close clock source.
	_SET(t_TCCRnB, WGM52); _CLEAR(t_TCCRnB, WGM53); //CTC mode, TOP: OCR5A;
	TCCR5B = t_TCCRnB;
	TCCR5A = 0;
	TCCR5C = 0;
	TCNT5H = 0; //set TCNT5 to 0
	TCNT5L = 0; //set TCNT5 to 0
	TIMSK5 |= _BV(OCIE5A); // Output Compare A Match Interrupt Enable
	_set_t5_isi(CTC_ISI);
}


inline static void _t5_stop(){
	TCCR5B &= ~PRE_SCALE_MASK;
	TCNT5H = 0; //set TCNT5 to 0
	TCNT5L = 0; //set TCNT5 to 0
}

inline void _t5_start(byte pre_scale){
	TCCR5B |= pre_scale; //set prescale
}
void t5_init_global()
{
	t5_ON_DUR = 0;
	t5_OFF_DUR = 0;
	t5_PHASE_COUT_NUM = 0;
	t5_DUR_COUT_NUM = 0;
 	t5_TIMEPASSED = 0;
	t5_PRETRG_DELAY_COUT_NUM = 0;
	t5_FLIPTIME = 0;
	t5_REAL_PW = 0;
	t5_POWER_RAMPSTEP = 0;
	t5_RAMPDOWN_FLAG = 0;
	t5_RAMPCOUNTER = 0;
	t5_RAMPSTEPNUM = 0;
}

void t5_operator()
{
 	t5_TIMEPASSED++;
	if(t5_TIMEPASSED < t5_PRETRG_DELAY_COUT_NUM) return;
	else if (t5_TIMEPASSED == t5_PRETRG_DELAY_COUT_NUM)
	{
		PWM_write4A(t5_REAL_PW);
	}

	if (t5_TIMEPASSED < (t5_PRETRG_DELAY_COUT_NUM + t5_DUR_COUT_NUM))
	{
		if(t5_TIMEPASSED < t5_FLIPTIME) return;
		else if((t5_CUR_STATUS == TRG_ON) && (t5_OFF_DUR > 0))
		{
			t5_CUR_STATUS = TRG_OFF;
			t5_FLIPTIME = t5_FLIPTIME + t5_OFF_DUR;
			PWM_write4A(0);
		}
		else if((t5_CUR_STATUS == TRG_OFF) && (t5_ON_DUR > 0))
		{
			t5_CUR_STATUS = TRG_ON;
			t5_FLIPTIME = t5_FLIPTIME + t5_ON_DUR;
			PWM_write4A(t5_REAL_PW);
		}
	}
	else if (1 == t5_RAMPDOWN_FLAG)
	{
		if ((t5_REAL_PW > t5_POWER_RAMPSTEP)&&(t5_RAMPCOUNTER < t5_RAMPSTEPNUM))
		{
			t5_REAL_PW = t5_REAL_PW - t5_POWER_RAMPSTEP;
			PWM_write4A(t5_REAL_PW);
			t5_RAMPCOUNTER++;
		}
		else
		{
			_t5_stop();
			PWM_write4A(0);
			t5_init_global();
		}
	}
	else
	{
		_t5_stop();
		PWM_write4A(0);
		t5_init_global();
	}
}

ISR(TIMER5_COMPA_vect)
{
	t5_operator();
}

void PWM_PULSE_Class::p1_multipulses(unsigned long duration, float fq, unsigned long p_width, unsigned long pre_trg_delay,int power)
{
	float CYCLE = 1000 / fq;
	if (power > 100)
	{
		Serial.println("Error in Pulse, power can't be greater than 100.");
		return;
	}
	t5_REAL_PW = power * 8;
	if (duration == 0) return;

	//t5_PIN_ = pin;
	t5_DUR_COUT_NUM = duration * CTC_FACTOR;
	t5_PRETRG_DELAY_COUT_NUM = pre_trg_delay * CTC_FACTOR;

	t5_ON_DUR = (p_width > 0) ? (p_width * CTC_FACTOR) : 0;
	t5_OFF_DUR = (CYCLE > p_width) ? ((CYCLE - p_width) * CTC_FACTOR) : 0;

	t5_PHASE_COUT_NUM = t5_ON_DUR;
	t5_CUR_STATUS = TRG_ON;
	t5_FLIPTIME = t5_PRETRG_DELAY_COUT_NUM + t5_ON_DUR;
	if (t5_PRETRG_DELAY_COUT_NUM == 0) PWM_write4A(t5_REAL_PW);
	_t5_start(PRE_SCALE_64DIV);
}



void PWM_PULSE_Class::p1_multipulses_ramp(unsigned long duration, float fq, unsigned long p_width, unsigned long pre_trg_delay, int power, int rampdown_stepnum)
{
	t5_RAMPSTEPNUM = rampdown_stepnum;
	t5_POWER_RAMPSTEP = ramp_cal(power * 8, rampdown_stepnum);
	t5_RAMPCOUNTER = 0;
	t5_RAMPDOWN_FLAG = 1;
	p1_multipulses(duration, fq, p_width, pre_trg_delay, power);
}

void PWM_PULSE_Class::p1_constant( uint32_t duration, unsigned long pre_trg_delay, int power)
{
	float fq = (float)1000 / 2 / (float)duration;
	p1_multipulses(duration, fq, duration, pre_trg_delay, power);
}

void PWM_PULSE_Class::p1_constant_ramp( uint32_t duration, unsigned long pre_trg_delay, int power, int rampdown_stepnum)
{
	t5_RAMPSTEPNUM = rampdown_stepnum;
	t5_POWER_RAMPSTEP = ramp_cal(power * 8, rampdown_stepnum);
	t5_RAMPCOUNTER = 0;
	t5_RAMPDOWN_FLAG = 1;
	p1_constant(duration, pre_trg_delay, power);
}

void PWM_PULSE_Class::p1_cancel()
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		_t5_stop();
		PWM_write4A(0);
		t5_init_global();
	}
}

void PWM_PULSE_Class::p1_cancel_ramp(int rampdown_stepnum)
{
	t5_PRETRG_DELAY_COUT_NUM = 0;
	t5_RAMPSTEPNUM = rampdown_stepnum;
	t5_POWER_RAMPSTEP = ramp_cal(t5_REAL_PW,rampdown_stepnum);
	t5_RAMPCOUNTER = 0;
	t5_RAMPDOWN_FLAG = 1;
	t5_DUR_COUT_NUM = 0;
}

/***********p2***************/
/***********p2***************/
unsigned long t3_ON_DUR;
unsigned long t3_OFF_DUR;
unsigned long t3_PHASE_COUT_NUM;
unsigned long t3_DUR_COUT_NUM;
unsigned long t3_TIMEPASSED;
unsigned long t3_CUR_STATUS;
unsigned long t3_FLIPTIME;
unsigned long t3_PRETRG_DELAY_COUT_NUM;
float t3_REAL_PW;
float t3_POWER_RAMPSTEP;
unsigned long t3_RAMPCOUNTER;
int t3_RAMPSTEPNUM;
char t3_RAMPDOWN_FLAG;


inline static void _set_t3_isi(uint16_t isi){
	OCR3AH = isi >> 8;
	OCR3AL = isi;
}

inline static void t3_init(){
	t_TCCRnB = TCCR3B;
	_CLEAR(t_TCCRnB, CS30); _CLEAR(t_TCCRnB, CS31); _CLEAR(t_TCCRnB, CS32); //Close clock source.
	_SET(t_TCCRnB, WGM32); _CLEAR(t_TCCRnB, WGM33); //CTC mode, TOP: OCR3A;
	TCCR3B = t_TCCRnB;
	TCCR3A = 0;
	TCCR3C = 0;
	TCNT3H = 0; //set TCNT3 to 0
	TCNT3L = 0; //set TCNT3 to 0
	TIMSK3 |= _BV(OCIE3A); // Output Compare A Match Interrupt Enable
	_set_t3_isi(CTC_ISI);
}


inline static void _t3_stop(){
	TCCR3B &= ~PRE_SCALE_MASK;
	TCNT3H = 0; //set TCNT3 to 0
	TCNT3L = 0; //set TCNT3 to 0
}

inline void _t3_start(byte pre_scale){
	TCCR3B |= pre_scale; //set prescale
}
void t3_init_global()
{
	t3_ON_DUR = 0;
	t3_OFF_DUR = 0;
	t3_PHASE_COUT_NUM = 0;
	t3_DUR_COUT_NUM = 0;
 	t3_TIMEPASSED = 0;
	t3_PRETRG_DELAY_COUT_NUM = 0;
	t3_FLIPTIME = 0;
	t3_REAL_PW = 0;
	t3_POWER_RAMPSTEP = 0;
	t3_RAMPDOWN_FLAG = 0;
	t3_RAMPCOUNTER = 0;
	t3_RAMPSTEPNUM = 0;
}

void t3_operator()
{
 	t3_TIMEPASSED++;
	if(t3_TIMEPASSED < t3_PRETRG_DELAY_COUT_NUM) return;
	else if (t3_TIMEPASSED == t3_PRETRG_DELAY_COUT_NUM)
	{
		PWM_write4B(t3_REAL_PW);
	}

	if (t3_TIMEPASSED < (t3_PRETRG_DELAY_COUT_NUM + t3_DUR_COUT_NUM))
	{
		if(t3_TIMEPASSED < t3_FLIPTIME) return;
		else if((t3_CUR_STATUS == TRG_ON) && (t3_OFF_DUR > 0))
		{
			t3_CUR_STATUS = TRG_OFF;
			t3_FLIPTIME = t3_FLIPTIME + t3_OFF_DUR;
			PWM_write4B(0);
		}
		else if((t3_CUR_STATUS == TRG_OFF) && (t3_ON_DUR > 0))
		{
			t3_CUR_STATUS = TRG_ON;
			t3_FLIPTIME = t3_FLIPTIME + t3_ON_DUR;
			PWM_write4B(t3_REAL_PW);
		}
	}
	else if (1 == t3_RAMPDOWN_FLAG)
	{
		if ((t3_REAL_PW > t3_POWER_RAMPSTEP)&&(t3_RAMPCOUNTER < t3_RAMPSTEPNUM))
		{
			t3_REAL_PW = t3_REAL_PW - t3_POWER_RAMPSTEP;
			PWM_write4B(t3_REAL_PW);
			t3_RAMPCOUNTER++;
		}
		else
		{
			_t3_stop();
			PWM_write4B(0);
			t3_init_global();
		}
	}
	else
	{
		_t3_stop();
		PWM_write4B(0);
		t3_init_global();
	}
}

ISR(TIMER3_COMPA_vect)
{
	t3_operator();
}

void PWM_PULSE_Class::p2_multipulses(unsigned long duration, float fq, unsigned long p_width, unsigned long pre_trg_delay,int power)
{
	float CYCLE = 1000 / fq;
	if (power > 100)
	{
		Serial.println("Error in Pulse, power can't be greater than 100.");
		return;
	}
	t3_REAL_PW = power * 8;
	if (duration == 0) return;

	t3_DUR_COUT_NUM = duration * CTC_FACTOR;
	t3_PRETRG_DELAY_COUT_NUM = pre_trg_delay * CTC_FACTOR;

	t3_ON_DUR = (p_width > 0) ? (p_width * CTC_FACTOR) : 0;
	t3_OFF_DUR = (CYCLE > p_width) ? ((CYCLE - p_width) * CTC_FACTOR) : 0;

	t3_PHASE_COUT_NUM = t3_ON_DUR;
	t3_CUR_STATUS = TRG_ON;
	t3_FLIPTIME = t3_PRETRG_DELAY_COUT_NUM + t3_ON_DUR;
	if (t3_PRETRG_DELAY_COUT_NUM == 0) PWM_write4B(t3_REAL_PW);
	_t3_start(PRE_SCALE_64DIV);
}



void PWM_PULSE_Class::p2_multipulses_ramp(unsigned long duration, float fq, unsigned long p_width, unsigned long pre_trg_delay, int power, int rampdown_stepnum)
{
	t3_RAMPSTEPNUM = rampdown_stepnum;
	t3_POWER_RAMPSTEP = ramp_cal(power * 8, rampdown_stepnum);
	t3_RAMPCOUNTER = 0;
	t3_RAMPDOWN_FLAG = 1;
	p2_multipulses(duration, fq, p_width, pre_trg_delay, power);
}

void PWM_PULSE_Class::p2_constant( uint32_t duration, unsigned long pre_trg_delay, int power)
{
	float fq = (float)1000 / 2 / (float)duration;
	p2_multipulses(duration, fq, duration, pre_trg_delay, power);
}

void PWM_PULSE_Class::p2_constant_ramp( uint32_t duration, unsigned long pre_trg_delay, int power, int rampdown_stepnum)
{
	t3_RAMPSTEPNUM = rampdown_stepnum;
	t3_POWER_RAMPSTEP = ramp_cal(power * 8, rampdown_stepnum);
	t3_RAMPCOUNTER = 0;
	t3_RAMPDOWN_FLAG = 1;
	p2_constant(duration, pre_trg_delay, power);
}

void PWM_PULSE_Class::p2_cancel()
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		_t3_stop();
		PWM_write4B(0);
		t3_init_global();
	}
}

void PWM_PULSE_Class::p2_cancel_ramp(int rampdown_stepnum)
{
	t3_PRETRG_DELAY_COUT_NUM = 0;
	t3_RAMPSTEPNUM = rampdown_stepnum;
	t3_POWER_RAMPSTEP = ramp_cal(t3_REAL_PW,rampdown_stepnum);
	t3_RAMPCOUNTER = 0;
	t3_RAMPDOWN_FLAG = 1;
	t3_DUR_COUT_NUM = 0;
}


/***********p3***************/
/***********p3***************/
unsigned long t1_ON_DUR;
unsigned long t1_OFF_DUR;
unsigned long t1_PHASE_COUT_NUM;
unsigned long t1_DUR_COUT_NUM;
unsigned long t1_TIMEPASSED;
unsigned long t1_CUR_STATUS;
unsigned long t1_FLIPTIME;
unsigned long t1_PRETRG_DELAY_COUT_NUM;
float t1_REAL_PW;
float t1_POWER_RAMPSTEP;
unsigned long t1_RAMPCOUNTER;
int t1_RAMPSTEPNUM;
char t1_RAMPDOWN_FLAG;


inline static void _set_t1_isi(uint16_t isi){
	OCR1AH = isi >> 8;
	OCR1AL = isi;
}

inline static void t1_init(){
	t_TCCRnB = TCCR1B;
	_CLEAR(t_TCCRnB, CS10); _CLEAR(t_TCCRnB, CS11); _CLEAR(t_TCCRnB, CS12); //Close clock source.
	_SET(t_TCCRnB, WGM12); _CLEAR(t_TCCRnB, WGM13); //CTC mode, TOP: OCR1A;
	TCCR1B = t_TCCRnB;
	TCCR1A = 0;
	TCCR1C = 0;
	TCNT1H = 0; //set TCNT1 to 0
	TCNT1L = 0; //set TCNT1 to 0
	TIMSK1 |= _BV(OCIE1A); // Output Compare A Match Interrupt Enable
	_set_t1_isi(CTC_ISI);
}


inline static void _t1_stop(){
	TCCR1B &= ~PRE_SCALE_MASK;
	TCNT1H = 0; //set TCNT1 to 0
	TCNT1L = 0; //set TCNT1 to 0
}

inline void _t1_start(byte pre_scale){
	TCCR1B |= pre_scale; //set prescale
}
void t1_init_global()
{
	t1_ON_DUR = 0;
	t1_OFF_DUR = 0;
	t1_PHASE_COUT_NUM = 0;
	t1_DUR_COUT_NUM = 0;
 	t1_TIMEPASSED = 0;
	t1_PRETRG_DELAY_COUT_NUM = 0;
	t1_FLIPTIME = 0;
	t1_REAL_PW = 0;
	t1_POWER_RAMPSTEP = 0;
	t1_RAMPDOWN_FLAG = 0;
	t1_RAMPCOUNTER = 0;
	t1_RAMPSTEPNUM = 0;
}

void t1_operator()
{
 	t1_TIMEPASSED++;
	if(t1_TIMEPASSED < t1_PRETRG_DELAY_COUT_NUM) return;
	else if (t1_TIMEPASSED == t1_PRETRG_DELAY_COUT_NUM)
	{
		PWM_write4C(t1_REAL_PW);
	}

	if (t1_TIMEPASSED < (t1_PRETRG_DELAY_COUT_NUM + t1_DUR_COUT_NUM))
	{
		if(t1_TIMEPASSED < t1_FLIPTIME) return;
		else if((t1_CUR_STATUS == TRG_ON) && (t1_OFF_DUR > 0))
		{
			t1_CUR_STATUS = TRG_OFF;
			t1_FLIPTIME = t1_FLIPTIME + t1_OFF_DUR;
			PWM_write4C(0);
		}
		else if((t1_CUR_STATUS == TRG_OFF) && (t1_ON_DUR > 0))
		{
			t1_CUR_STATUS = TRG_ON;
			t1_FLIPTIME = t1_FLIPTIME + t1_ON_DUR;
			PWM_write4C(t1_REAL_PW);
		}
	}
	else if (1 == t1_RAMPDOWN_FLAG)
	{
		if ((t1_REAL_PW > t1_POWER_RAMPSTEP)&&(t1_RAMPCOUNTER < t1_RAMPSTEPNUM))
		{
			t1_REAL_PW = t1_REAL_PW - t1_POWER_RAMPSTEP;
			PWM_write4C(t1_REAL_PW);
			t1_RAMPCOUNTER++;
		}
		else
		{
			_t1_stop();
			PWM_write4C(0);
			t1_init_global();
		}
	}
	else
	{
		_t1_stop();
		PWM_write4C(0);
		t1_init_global();
	}
}

ISR(TIMER1_COMPA_vect)
{
	t1_operator();
}

void PWM_PULSE_Class::p3_multipulses(unsigned long duration, float fq, unsigned long p_width, unsigned long pre_trg_delay,int power)
{
	float CYCLE = 1000 / fq;
	if (power > 100)
	{
		Serial.println("Error in Pulse, power can't be greater than 100.");
		return;
	}
	t1_REAL_PW = power * 8;
	if (duration == 0) return;

	t1_DUR_COUT_NUM = duration * CTC_FACTOR;
	t1_PRETRG_DELAY_COUT_NUM = pre_trg_delay * CTC_FACTOR;

	t1_ON_DUR = (p_width > 0) ? (p_width * CTC_FACTOR) : 0;
	t1_OFF_DUR = (CYCLE > p_width) ? ((CYCLE - p_width) * CTC_FACTOR) : 0;

	t1_PHASE_COUT_NUM = t1_ON_DUR;
	t1_CUR_STATUS = TRG_ON;
	t1_FLIPTIME = t1_PRETRG_DELAY_COUT_NUM + t1_ON_DUR;
	if (t1_PRETRG_DELAY_COUT_NUM == 0) PWM_write4C(t1_REAL_PW);
	_t1_start(PRE_SCALE_64DIV);
}



void PWM_PULSE_Class::p3_multipulses_ramp(unsigned long duration, float fq, unsigned long p_width, unsigned long pre_trg_delay, int power, int rampdown_stepnum)
{
	t1_RAMPSTEPNUM = rampdown_stepnum;
	t1_POWER_RAMPSTEP = ramp_cal(power * 8, rampdown_stepnum);
	t1_RAMPCOUNTER = 0;
	t1_RAMPDOWN_FLAG = 1;
	p3_multipulses(duration, fq, p_width, pre_trg_delay, power);
}

void PWM_PULSE_Class::p3_constant( uint32_t duration, unsigned long pre_trg_delay, int power)
{
	float fq = (float)1000 / 2 / (float)duration;
	p3_multipulses(duration, fq, duration, pre_trg_delay, power);
}

void PWM_PULSE_Class::p3_constant_ramp( uint32_t duration, unsigned long pre_trg_delay, int power, int rampdown_stepnum)
{
	t1_RAMPSTEPNUM = rampdown_stepnum;
	t1_POWER_RAMPSTEP = ramp_cal(power * 8, rampdown_stepnum);
	t1_RAMPCOUNTER = 0;
	t1_RAMPDOWN_FLAG = 1;
	p3_constant(duration, pre_trg_delay, power);
}

void PWM_PULSE_Class::p3_cancel()
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		_t1_stop();
		PWM_write4C(0);
		t1_init_global();
	}
}

void PWM_PULSE_Class::p3_cancel_ramp(int rampdown_stepnum)
{
	t1_PRETRG_DELAY_COUT_NUM = 0;
	t1_RAMPSTEPNUM = rampdown_stepnum;
	t1_POWER_RAMPSTEP = ramp_cal(t1_REAL_PW,rampdown_stepnum);
	t1_RAMPCOUNTER = 0;
	t1_RAMPDOWN_FLAG = 1;
	t1_DUR_COUT_NUM = 0;
}


/***************init********************/
/***************init********************/
void PWM_PULSE_Class::init()
{
	p1_init();
	p2_init();
	p3_init();
	// Preserve the original init() behavior. Per-channel init preserves SREG.
	sei();

}


void PWM_PULSE_Class::p1_init()
{
	static bool initialized = false;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		if (!initialized)
		{
			t4_PWM_init();
			t5_init_global();
			t5_init();
			TIFR5 = _BV(OCF5A); // Clear a stale compare interrupt before use.
			PWM_write4A(0);
			_CLEAR(PORTH, PORTH3);
			_SET(DDRH, DDH3);
			_SET(TCCR4A, COM4A1);
			_CLEAR(TCCR4A, COM4A0);
			initialized = true;
		}
	}
}

void PWM_PULSE_Class::p2_init()
{
	static bool initialized = false;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		if (!initialized)
		{
			t4_PWM_init();
			t3_init_global();
			t3_init();
			TIFR3 = _BV(OCF3A); // Clear a stale compare interrupt before use.
			PWM_write4B(0);
			_CLEAR(PORTH, PORTH4);
			_SET(DDRH, DDH4);
			_SET(TCCR4A, COM4B1);
			_CLEAR(TCCR4A, COM4B0);
			initialized = true;
		}
	}
}

void PWM_PULSE_Class::p3_init()
{
	static bool initialized = false;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		if (!initialized)
		{
			t4_PWM_init();
			t1_init_global();
			t1_init();
			TIFR1 = _BV(OCF1A); // Clear a stale compare interrupt before use.
			PWM_write4C(0);
			_CLEAR(PORTH, PORTH5);
			_SET(DDRH, DDH5);
			_SET(TCCR4A, COM4C1);
			_CLEAR(TCCR4A, COM4C0);
			initialized = true;
		}
	}
}

PWM_PULSE_Class PWM_PULSE;
