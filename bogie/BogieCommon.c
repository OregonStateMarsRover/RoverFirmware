/*BogieCommon.c
 *Authors: Mike Fortner, Chris Newman
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
	while( !(OSC.STATUS & 0x08) ) {
		PORTD.OUTTGL = 0x20;	// toggle red LED
		OSC.XOSCCTRL = 0b11001011;	// set up oscillator for 16 Mhz
		OSC.CTRL |= 0x08;	// Start the external clock.
		_delay_ms(50);
	}
	PORTD.OUTSET = 0x20;	// turn off red LED
	OSC.PLLCTRL = 0b11000001;	//Set PLL to use external clock
	
	CCP = 0xD8;	// enable change of protected registers
	CLK.CTRL = 3;		// use external clock

	CCP = 0xD8;	// enable change of protected registers
	CLK.PSCTRL = 0;		// disable prescaler
}

