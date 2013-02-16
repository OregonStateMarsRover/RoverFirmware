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
#include "stdio.h"
#include "string.h"

#define GAIN_SCALAR 10

/* Setup the realtime clock.
 * Use a period in miliseconds.
 * Make overflow a low-level interrupt
 */
void setup_rtc( uint16_t period ) {
	/* Enable the ULP clock */
	CLK.RTCCTRL = 0x01;

	/* Setup the RTC */
	RTC.PER = period - 1;
	RTC.CNT = 0;
	RTC.INTCTRL = 0x01;	// low-level interrupt
	RTC.CTRL = 1;	// 1 ms resolution

	/* Enable low-level interrupts with round-robin */
	PMIC.CTRL |= 0x81;
	PORTD.OUTSET = 0x10;	// enable green status LED

}



void pid_setup( struct pid * settings, int16_t p, int16_t i, int16_t d, int16_t ramp, uint8_t dt) {

	settings->p = p;
	settings->i = i;
	// no d term currently
	settings->ramp = ramp;
	settings->dt = dt;
}

void pid_speed_controller( struct pid * val ) {
	int16_t output = 0;
	output = (val->setpoint - val->pv) * val->p;
	output /= GAIN_SCALAR;

	/* Ramp is basically our ceiling of our proportional
	 * gain, so it acts smoothely when there's a long way
	 * to go, but will get there quickly when it's close
	 */
	if( output > val->ramp ) output = val->ramp;
	else if( output < -val->ramp) output = -val->ramp;

	// Account for time
	output *= val->dt;

	/* catch overflows */
	int16_t newout = output + val->output;
	if( output > 0 && val->output > 0 && newout < 0 ) {
		newout = INT16_MAX;
	} else if( output < 0 && val->output < 0 && newout > 0 ) {
		newout = INT16_MIN;
	}
	val->output = newout;


}


char pid_msg[60];
unsigned short pmsg_len;

void print_pid( struct pid * val ) {
	snprintf( pid_msg, 60, "setpoint = %d, pv = %d, out = %d\r\n",
			val->setpoint, val->pv, val->output );
	pmsg_len = strlen( pid_msg);
	USART_Write( &bogie.bb, (uint8_t *)pid_msg, pmsg_len );
}


ISR(RTC_OVF_vect) {
	// Do PID stuff
	speed_pid.setpoint = bogie.drive;
	/* Since wheel speed has no direction yet, we have to
	 * implement it ourselves.
	 */
	speed_pid.pv = ((speed_pid.output > 0) ? 
		wheel_speed() : -wheel_speed());

	pid_speed_controller( &speed_pid );

	/* We scale down the set because the PID needs a higher resolution
	 * for it to accumulate smoothely. */
	drive_set( speed_pid.output >> 8);

//	PORTD.OUTTGL = GREEN;

//	print_pid( &speed_pid );
}
