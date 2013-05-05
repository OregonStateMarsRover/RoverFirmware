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
	PORTC.DIRCLR = 0x03 << 6;

	// Set the pin configuration for both to low-level sense
	PORTCFG.MPCMASK = 0x03 << 6;
	PORTC.PIN0CTRL = (PORTC.PIN0CTRL & ~PORT_ISC_gm) | PORT_ISC_LEVEL_gc;

	// Set the first pin as a multiplexer for the event channel.
	EVSYS.CH0MUX = 0x60 | 6;	// PORTC, pin 6

	// Enable quadrature decoding and digital filtering on the event channel.
	EVSYS.CH0CTRL = EVSYS_QDEN_bm | EVSYS_DIGFILT_2SAMPLES_gc;

	// Set the quadrature decoding as the event action for a timer/counter
	// Also, select the event channel as the event source for the timer/counter
	TCC1.CTRLD = TC_EVACT_QDEC_gc | 0x80;	// QDEC on channel 0

	// We leave the period at the default value of 0xFFFF.
	TCC1.PER = 0xFFFF;

	// Enable the timer/counter w/out clock prescaling
	TCC1.CTRLA = 0x01;	// set clock to no prescaling


	/* Period measurement for the magnetic encoder to come later */
}


int16_t get_turn( void ) {
	return TCC1.CNT;
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
