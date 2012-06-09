#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdlib.h>

#define BTNSKIP 4

volatile unsigned char cmd_wait = 0;
volatile unsigned short command = 0;
volatile enum {
	CMD_NONE = 0, CMD_NO1, CMD_NO2, CMD_NO3,
	CMD_MAINS_ON, CMD_MAINS_OFF, CMD_LIGHT_ON, CMD_LIGHT_OFF,
	CMD_LIGHT_10P, CMD_LIGHT_20P, CMD_LIGHT_40P, CMD_LIGHT_60P,
	CMD_LIGHT_STROBE, CMD_FADE_UP, CMD_FADE_DOWN
} COMMAND = 0;
volatile enum {
	L_ON, L_OFF, L_10P, L_20P, L_40P, L_60P, L_STROBE, L_FUP, L_FDOWN
} light = L_OFF;

int main(void)
{
	MCUSR &= ~_BV(WDRF);
	WDTCSR = _BV(WDCE) | _BV(WDE);
	WDTCSR = _BV(WDE) | _BV(WDP3);

	ACSR |= _BV(ACD);

	DDRB = _BV(DDB5) | _BV(DDB6);
	PORTB = _BV(PB0) | _BV(PB1) | _BV(PB2);

	DDRD = _BV(DDD4) | _BV(DDD5);
	PORTD = _BV(PD2);

	OCR1A = 0xff;
	TCCR1A = 0;
	TCCR1B = _BV(WGM12) | _BV(CS12);
	TIMSK = _BV(OCIE1A);

	sei();

	while (1) {
		MCUSR |= _BV(SE);
		asm("sleep");
		asm("wdr");
	}

	return 0;
}

static void forward_command(void)
{
	for (int i = 0; i < command; i++) {
		PORTB |= _BV(PB5);
		for (int j = 0; j < 100; j++)
			asm("wdr");
		PORTB &= ~_BV(PB5);
		for (int j = 0; j < 100; j++)
			asm("wdr");
	}
}

static void run_command(void)
{
	if (command <= CMD_FADE_DOWN) {
	switch (command) {
		case CMD_NONE:
		case CMD_NO1:
		case CMD_NO2:
		case CMD_NO3:
			break;
		case CMD_MAINS_ON:
			PORTD |= _BV(PD4);
			break;
		case CMD_MAINS_OFF:
			PORTD &= ~_BV(PD4);
			break;
		case CMD_LIGHT_ON:
			light = L_ON;
			TCCR0A = TCCR0B = 0;
			PORTD |= _BV(PD5);
			break;
		case CMD_LIGHT_OFF:
			light = L_OFF;
			TCCR0A = TCCR0B = 0;
			PORTD &= ~_BV(PD5);
			break;
		case CMD_LIGHT_10P:
			light = L_10P;
			OCR0B = 26;
			TCCR0A = _BV(COM0B1) | _BV(WGM01) | _BV(WGM00);
			TCCR0B = _BV(CS00);
			break;
		case CMD_LIGHT_20P:
			light = L_20P;
			OCR0B = 52;
			TCCR0A = _BV(COM0B1) | _BV(WGM01) | _BV(WGM00);
			TCCR0B = _BV(CS00);
			break;
		case CMD_LIGHT_40P:
			light = L_40P;
			OCR0B = 104;
			TCCR0A = _BV(COM0B1) | _BV(WGM01) | _BV(WGM00);
			TCCR0B = _BV(CS00);
			break;
		case CMD_LIGHT_60P:
			light = L_60P;
			OCR0B = 156;
			TCCR0A = _BV(COM0B1) | _BV(WGM01) | _BV(WGM00);
			TCCR0B = _BV(CS00);
			break;
		case CMD_LIGHT_STROBE:
			light = L_STROBE;
			OCR0B = 128;
			TCCR0A = _BV(COM0B1) | _BV(WGM01) | _BV(WGM00);
			TCCR0B = _BV(CS02);
			break;
		case CMD_FADE_UP:
			light = L_FUP;
			OCR0B = 0;
			TCCR0A = _BV(COM0B1) | _BV(WGM01) | _BV(WGM00);
			TCCR0B = _BV(CS00);
			break;
		case CMD_FADE_DOWN:
			light = L_FDOWN;
			OCR0B = 255;
			TCCR0A = _BV(COM0B1) | _BV(WGM01) | _BV(WGM00);
			TCCR0B = _BV(CS00);
			break;
	}
	}
	else {
		forward_command();
	}
	command = CMD_NONE;
}

ISR(INT0_vect)
{
	cli();
	command++;
	cmd_wait = 6;
}

ISR(TIMER1_COMPA_vect)
{
	static unsigned char skip = 0;
	static unsigned char boot = 100;
	static unsigned char fadestep = 3;
	cli();

	if (boot) {
		boot--;

		if (boot == 1) {
			MCUCR = _BV(ISC01) | _BV(ISC00);
			GIMSK = _BV(INT0);
		}
		return;
	}

	if (cmd_wait) {
		cmd_wait--;
		return;
	}
	else if (command)
		run_command();

	if (light == L_FUP) {
		if (OCR0B == 255)
			command = CMD_LIGHT_ON;
		else {
			if (!fadestep) {
				OCR0B++;
				fadestep = 2;
			}
			fadestep--;
		}
	}
	if (light == L_FDOWN) {
		if (OCR0B == 0)
			command = CMD_LIGHT_OFF;
		else {
			if (!fadestep) {
				OCR0B--;
				fadestep = 3;
			}
			fadestep--;
		}
	}

	if (skip)
		skip--;
	else if (~PINB & _BV(PB0)) {
		skip = BTNSKIP;
		PIND |= _BV(PD4);
	}
	else if (~PINB & _BV(PB1)) {
		skip = BTNSKIP;
		if (light == L_OFF)
			command = CMD_LIGHT_ON;
		else
			command = CMD_LIGHT_OFF;
	}
	else if (~PINB & _BV(PB2)) {
		skip = BTNSKIP;
		switch (light) {
			case L_ON:
			case L_OFF:
				command = CMD_LIGHT_10P;
				break;
			case L_10P:
				command = CMD_LIGHT_20P;
				break;
			case L_20P:
				command = CMD_LIGHT_40P;
				break;
			case L_40P:
				command = CMD_LIGHT_60P;
				break;
			case L_60P:
				command = CMD_LIGHT_STROBE;
				break;
			case L_STROBE:
				command = CMD_LIGHT_OFF;
				break;
		}
	}
}
