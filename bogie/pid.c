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





ISR(RTC_OVF_vect) {
	// Do PID stuff
	PORTD.OUTTGL = 0x10;
}
