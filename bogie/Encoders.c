/*
 * Encoders.c
 *
 * Created: 4/24/2012 4:18:17 PM
 *  Author: Marshal Horn
 */ 

#include "Encoders.h"


void encoders_init() {
	/*** initialize min and max for actuator encoder ***/
	/*** Initialize Quadrature Decoder for Actuator encoder***/
	/*
	
	QDEC_Total_Setup(&PORTC,                    //PORT_t * qPort
	                 6,                         //uint8_t qPin
	                 false,                     //bool invIO
	                 0,                         //uint8_t qEvMux
	                 EVSYS_CHMUX_PORTC_PIN6_gc, //EVSYS_CHMUX_t qPinInput
	                 false,                     //bool useIndex
	                 EVSYS_QDIRM_00_gc,         //EVSYS_QDIRM_t qIndexState
	                 &TCC0,                     //TC0_t * qTimer
	                 TC_EVSEL_CH0_gc,           //TC_EVSEL_t qEventChannel
	                 ACTUATOR_QUADRATURE_LINECOUNT);   //uint8_t lineCount
	
					 */
	/*** Initialize Counter for Drive encoder***/
	
	wheel_enc_init();
}

void wheel_enc_init( void ) {
	/* Setup the event channel for channel A of the encoder */
	PORTC.DIRCLR = PIN4_bm;				  //set PC5 to input
	PORTC.PIN4CTRL |= PORT_ISC_RISING_gc;  //set PC5 to trigger events on rising edges
	
	EVSYS.CH2MUX = EVSYS_CHMUX_PORTC_PIN4_gc;  //set PC4/Pin 14 to input for event channel 2

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
	TCC1.CTRLB = 0x10;	

	/* Set timer to use event channel 2 for frequency measurement */
	TCC1.CTRLD = 0xAA;	// FRQ mode, no delay, event channel 2

	/* Set prescaler to 1/1024.
	 * Based off a 16Mhz system clock, this means we can measure a period as
	 * long as 4ms, and as short as 64us.
	 */
	TCC1.CTRLA = 0x07;

}

uint16_t get_actuator_pos()
{
	return 0;
}

int16_t wheel_speed() {
	// Doesn't account for timer clock speed yet
	uint16_t speed = 0xFFFF / TCC1.CCA;
	return speed;
}

