/*
 * Encoders.c
 *
 * Created: 4/24/2012 4:18:17 PM
 *  Author: Marshal Horn
 */ 


/* For the rotation encoders, we can expect a max frequency of 30 khz,
 * and a max range of 84,960 for a full rotation.
 * WARNING: This is larger than the 16-bit counter, so you'll need to 
 * handle an overflow interrupt in there if we choose to do a full rotation.
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
	wheel_enc_init();
	turn_enc_init();
}

/******************************************
 ************ WHEEL ENCODER ***************
 ******************************************/

void wheel_enc_init( void ) {
	/* Setup the event channel for channel A of the encoder */
	PORTC.DIRCLR = PIN4_bm;				  //set PC5 to input
	PORTC.PIN4CTRL |= PORT_ISC_RISING_gc;  //set PC5 to trigger events on rising edges
	
	EVSYS.CH2MUX = EVSYS_CHMUX_PORTC_PIN4_gc;  //set PC4/Pin 14 to input for event channel 2
	EVSYS.CH2CTRL = 0x03;	// set digital filtering for event

	/* For now, ignore direction, and just calculate speed.
	 * We want to reset the counter every time we receive a pulse.
	 * (For now, we can use a 16-bit counter, but we'll probably change it
	 * to an 8-bit counter later)
	 * Actually, we have two ways of doing this.
	 * 1: Measure the period between pulses
	 * 2: Measure the number of pulses in a given period
	 *
	 * Since 1 gives us faster response, it is the method we will pursue first.
	 * To do this, we capture the time of the input, and then reset the counter.
	 * Or, we can alternate capture channels, and measure the difference.
	 * However, the previous method seems to be more built-in.
	 * It is called "Frequency Capture".
	 * We can catch the overflow, if the motor is going too slow, and adjust the
	 * clock rate accordingly (if necessary).
	 */

	/* Enable Capture register A */ 
	TCC0.CTRLB = 0x10;	

	/* Set timer to use event channel 2 for frequency measurement */
	TCC0.CTRLD = 0xAA;	// FRQ mode, no delay, event channel 2

	/* Set prescaler to 1/1024.
	 * Based off a 16Mhz system clock, this means we can measure a period as
	 * long as 4ms, and as short as 64us.
	 */
	TCC0.CTRLA = 0x07;

}

int16_t get_speed() {
	int16_t speed = 0;
	/* If encoder has moved since we last checked */
	if( TCC0.INTFLAGS & 0x10 ) {
		TCC0.INTFLAGS = 0x10;	// clear interrupt flag

		/* Use the period to calculate speed.
		 * (Doesn't account for timer clock speed yet)
		 * The typecasting below is just to make extra
		 * sure we don't end up with a negative value due to
		 * the counter exceeding 0x7FFF.
		 */
		speed = (int16_t)(MAGNETIC_ENCODER_SCALER / 
				(uint16_t)TCC0.CCA);
	//} else {
		/* Encoder hasn't moved since we last checked.
		 * assume the wheel is stopped */
	//	speed = 0;
	}
	return speed;
}



/******************************************
 ************ TURN ENCODER ****************
 ******************************************/

void turn_enc_init() {
	/* Setup procedure for Quadrature encoding
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
	PORTC.INT1MASK = 0x40;
	PORTC.INTCTRL = (PORTC.INTCTRL & 0xF3) | 0x0C;	// high-level interrupt
	PMIC.CTRL |= 0x04;	// enable high-level interrpts

	// Enable quadrature decoding and digital filtering on the event channel.
	EVSYS.CH0CTRL = 0x0F; // eight-sample filter

	// Set the quadrature decoding as the event action for a timer/counter
	// Also, select the event channel as the event source for the timer/counter
	TCC1.CTRLD = 0x68; // event channel 0

	// Enable the timer/counter w/out clock prescaling
	TCC1.CTRLA = 0x01;	// set clock to no prescaling

}


ISR( PORTC_INT1_vect ) {
	EVSYS.DATA = PORTC.IN >> 7; // set count direction
	EVSYS.STROBE = 0x01;		// increment/decrement
}

int16_t get_turn()
{
	return TCC1.CNT;
}
