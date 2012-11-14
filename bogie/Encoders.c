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
	/*** initialize min and max for actuator encoder ***/
	/*** Initialize Quadrature Decoder for Actuator encoder***/
	
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
	
	/*** Initialize Counter for Drive encoder***/

	// I think the quadrature encoder is TC0 only?
	//QDEC_TC_Dec_Setup( &TCC1, TC_EVSEL_CH1_gc, DRIVE_ENCODER_LINECOUNT );
	
	/*
	PORTC.DIRCLR = PIN4_bm;				  //set PC4/Pin 14 to input
	PORTC.PIN4CTRL |= PORT_ISC_RISING_gc;  //set PC4/Pin 14 to trigger events on rising edges
	
	EVSYS.CH2MUX = EVSYS_CHMUX_PORTC_PIN4_gc;  //set PC4/Pin 14 to input for event channel 2
	
	TC1_t *motor_counter = &TCC1;
	TC1_ConfigClockSource( motor_counter, TC_CLKSEL_EVCH2_gc );  //set TCC1 to count events on channel 2
	*/
}

int16_t get_turn()
{
	return TCC0.CNT;
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
