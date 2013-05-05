/*
 * Encoders.c
 *
 * Created: 4/24/2012 4:18:17 PM
 *  Authors: mfortner, Marshal Horn
 */ 


/* Edits:
 * I think these funciton calls could pretty-much be just moved in here.
 * Read the datasheet on the encoders.  It doesn't look that hard.
 *
 * Two things to be wary of:
 * For the rotation encoders, we can expect a max frequency of 30 khz,
 * and a max range of 84,960.  WARNING: This is larger than the 16-bit
 * counter, so you'll need to handle an overflow interrupt in there.
 *
 * Also, we don't have documentation on the drive motors and magnetic
 * encoders for them.  Perhaps this is because the mechanical team
 * is going to change them?
 *
 *
 * LIMITS:
 * I think the limits and indexing functions should also go in this file.
 * That means Encoders.h should call Sabertooth.h, so if a limit is hit,
 * it can stop the motor.
 * However, I think the current sensing should be in its own file.
 * - Marshal
 */


#include "Encoders.h"
void encoders_init() {
	/* Setup procedure for Quadratuer encoding
	 * Described on page 51 of the XMEGA D4 Manual
	 */
	//Choose two successive pins on a port as QDEC phase input pins.
	//We are using pins 6 and 7 on PORTC.
	
	//Set them as inputs.
	PORTC.DIRCLR = 0xC0;

	// Set the pin configuration for both to low-level sense
	PORTC.PIN6CTRL = 0x01; // sense rising edge
	PORTC.PIN7CTRL = 0x03; // sense low level

	// Set the first pin as a multiplexer for the event channel.
	//EVSYS.CH0MUX = 0x60 + 0x08 + 6;	// PORTC, pin 6
	
	// For now, we're using interrupts to trigger the event
	PORTC.INT0MASK = 0x40;
	PORTC.INTCTRL = 0x03;	// high-level interrupt
	PMIC.CTRL |= 0x04;	// enable high-level interrpts

	// Enable quadrature decoding and digital filtering on the event channel.
	EVSYS.CH0CTRL = 0x0F; // eight-sample filter

	// Set the quadrature decoding as the event action for a timer/counter
	// Also, select the event channel as the event source for the timer/counter
	TCC1.CTRLD = 0x68; // event channel 0

	// Enable the timer/counter w/out clock prescaling
	TCC1.CTRLA = 0x01;	// set clock to no prescaling


	/* Period measurement for the magnetic encoder to come later */
}


ISR( PORTC_INT0_vect ) {
	EVSYS.DATA = PORTC.IN >> 7; // set count direction
	EVSYS.STROBE = 0x01;		// increment/decrement
}

/*! \brief This function return the direction of the counter/QDEC.
 *
 * \param qTimer      The timer used for QDEC.
 *
 * \retval CW_DIR     if clockwise/up counting,
 * \retval CCW_DIR    if counter clockwise/down counting.
 */
int16_t get_speed()
{
	/*
	int16_t speed = F_CPU / CC1BUFL;
	if (TCC1.CTRLFSET & TC1_DIR_bm){
		return speed;
	}else{
		return -speed;
	}
	*/
	return 0;
}
