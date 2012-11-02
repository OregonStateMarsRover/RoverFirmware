/*
 * Encoders.c
 *
 * Created: 4/24/2012 4:18:17 PM
 *  Author: mfortner
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
	
	PORTC.DIRCLR = PIN4_bm;				  //set PC4/Pin 14 to input
	PORTC.PIN4CTRL |= PORT_ISC_RISING_gc;  //set PC4/Pin 14 to trigger events on rising edges
	
	EVSYS.CH2MUX = EVSYS_CHMUX_PORTC_PIN4_gc;  //set PC4/Pin 14 to input for event channel 2
	
	TC1_t *motor_counter = &TCC1;
	TC1_ConfigClockSource( motor_counter, TC_CLKSEL_EVCH2_gc );  //set TCC1 to count events on channel 2
}

uint16_t get_actuator_pos()
{
	return 0;
}
