/*BogieCommon.c
 *Author: Marshal Horn
 */

#include "BogieCommon.h"


/* This function sets the clock speed for the microcontroller.
 * When you change it, make sure to change F_CPU to match.
 * Currently, it changes the main clock to the external crystal,
 * and makes the prescaler 1.
 * However, the 9600 baud setting in UART.c is probably still set
 * for the 2Mhz clock, and also needs to be adjusted.
 */
void set_clock( void ) {
	// Wait until the oscilator is stable
	PORTD.DIRSET = 0x20;
	uint8_t i = 0;
	uint8_t osc_ok = 1;
	while( !(OSC.STATUS & 0x08) ) {
		PORTD.OUTSET = 0x20;	// turn off LED
		OSC.XOSCCTRL = 0b11001011;	// set up oscillator for 16 Mhz
		OSC.CTRL |= 0x08;	// Start the external clock.
		_delay_ms( 7 );
		PORTD.OUTCLR= 0x20;	// turn on LED
		_delay_ms(10);
		++i;
		if( i > 4 ) {
			osc_ok = 0;
			break;
		}
	}
	if( osc_ok ) {
		PORTD.OUTSET = 0x20;	// turn off red LED
		OSC.PLLCTRL = 0b11000001;	//Set PLL to use external clock
		
		CCP = 0xD8;	// enable change of protected registers
		CLK.CTRL = 3;		// use external clock

		CCP = 0xD8;	// enable change of protected registers
		CLK.PSCTRL = 0;		// disable prescaler
	} else {
		/* External crystal isn't working, so we'll use the internal
		 * 32Mhz oscillator instead.  Make sure to prescale it down
		 * to 16Mhz so our UART baud rates aren't off.
		 */
		i = 0;
		while( !(OSC.STATUS & 0x02) ) {
			PORTD.OUTSET = 0x20;	// turn off LED
			OSC.CTRL |= 0x02;	// Start the 32Mhz oscillator
			_delay_ms( 7 );
			PORTD.OUTCLR= 0x20;	// turn on LED
			_delay_ms(15);
			++i;
		}
		PORTD.OUTSET = 0x20;	// turn off red LED
		
		CCP = 0xD8;	// enable change of protected registers
		CLK.CTRL = 1;		// use 32Mhz oscillator

		CCP = 0xD8;	// enable change of protected registers
		CLK.PSCTRL = 1;		// divide clock by 2
	}
}


/* Setup the realtime clock.
 * This is not for generating periodic events through interrupts, 
 * but for actually timing things.
 */
void setup_rtc( uint8_t prescaler ) {
	/* Enable the ULP clock */
	CLK.RTCCTRL = 0x01;

	/* Setup the RTC */
	RTC.CNT = 0;
	RTC.INTCTRL = 0;	// no interrupt
	RTC.CTRL = prescaler & 0x07;
}

/* This fuction uses global variables directly, since there is only
 * one instance of it in a program.
 */
void pid_steer( void ) {
	bogie.steer.pv = get_turn() * bogie.steer.pv_scale;
	//bogie.steer.output = bogie.steer.p * (bogie.steer.setpoint - bogie.steer.pv);
}
