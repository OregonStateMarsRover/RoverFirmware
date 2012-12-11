/* 
 * Author: Marshal Horn
 *
 * Date created: December 10th 2012
 *
 * Implementation of PID for the rover.
 * Keep in mind that there are two uses
 * for this PID - speed and position.
 * Because of their inherent differences,
 * there will be seperate functions for each.
 * So, these functions will probably not be applicable
 * to being used in more than one instance.
 */

#include "pid.h"

#define SPEED_MULTIPLIER (int16_t)0x20



/* Setup the realtime clock.
 * Use a period in miliseconds.
 * Make overflow a medium-level interrupt
 */
void setup_rtc( uint16_t period ) {
	/* Enable the ULP clock */
	CLK.RTCCTRL = 0x01;

	/* Setup the RTC */
	RTC.PER = period - 1;
	RTC.CNT = 0;
	RTC.INTCTRL = 0x02;	// medium-level interrupt
	RTC.CTRL = 1;	// 1 ms resolution

	/* Enable medium-level interrupts */
	PMIC.CTRL |= 0x02;
	PORTD.OUTSET = 0x10;	// enable green status LED

}



void pid_setup( struct pid * settings, int16_t p, int16_t i, int16_t d, int16_t ramp, uint8_t dt) {

	settings->p = p;
	settings->i = i;
	// no d term currently
	settings->ramp = ramp;
	settings->dt = dt;
}

/* This has none of the safeties or error checking
 * that I would expect a good PID algorithm to have,
 * but this is just for simple testing right now.
 */
void pid_speed_controller( struct pid * val ) {
	int16_t output = 0;
	output = (val->setpoint - val->pv) * val->p;
	output /= val->dt;
	val->output += output;
}




ISR(RTC_OVF_vect) {
	// Do PID stuff
	speed_pid.setpoint = bogie.drive * SPEED_MULTIPLIER;
	speed_pid.pv = wheel_speed();
	pid_speed_controller( &speed_pid );
	drive_set( speed_pid.output / SPEED_MULTIPLIER );
}
