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
	CCP = 0xD8;	// enable change of protected registers
	CLK.CTRL = 3;		// use external clock

	CCP = 0xD8;	// enable change of protected registers
	CLK.PSCTRL = 0;		// disable prescaler


}

uint8_t pid(PIDobject *pid, uint16_t desired, uint16_t actual)
{
	uint16_t p_error = desired - actual;

	uint16_t i_error = pid->sum_error / pid->dt;

	uint16_t d_error = p_error - pid->prev_error;

	pid->sum_error += p_error;
	pid->prev_error = p_error;

	return (uint8_t)(pid->p * p_error + pid->i * i_error + pid->d * d_error);
}

void pid_init( void ) {

	/*** Initialize Timer for PID loop***/
		

	//encoders_init();
	
	/*	Undefined references??
	TC0_t *loop_timer = &TCD0;
	TC_SetPeriod( loop_timer, 195U); //set period to (2000000/1024)(ticks/sec)/10(loops/sec) = 195 ticks/loop
	TC0_ConfigClockSource( loop_timer,  TC_CLKSEL_DIV1024_gc);  //set TCD0 to count the system clock. frequency should be 2,000,000 ticks/sec
	TC0_SetOverflowIntLevel( loop_timer, TC_OVFINTLVL_LO_gc);  //set TCD0 to trigger an interrupt when every overflow (100ms)
	*/

}


void parse_command(CommPacket *pkt)
{
	drive_set((int8_t)pkt->data[0]);
}

void reply(CommPacket *pkt) {
	CommPacket respPkt;
	respPkt.target = 2;
	respPkt.length = 2;
	unsigned char data[2];
	data[0] = 'B';
	data[1] = pkt->data[0];
	respPkt.data = data;

	CommSendPacket(&bogie.bb_com, &respPkt);
}


/* How is this going to work if we are using both our timer/counters
 * for encoders?  We may need to put our optical encoder on TC1, and
 * split TC0 so we can use half for timing the speed of the drive
 * motors, and half for timing our PID.
 * Or, perhaps we could use the real-time counter.
 */
/*
ISR(TCD0_OVF_vect)
{
 	PORTD.OUTTGL = 0b00110000;
 	drive_set(desired_speed); 
 	actuator_set(pid(&act_pid, desired_angle, get_actuator_pos()));
}
*/
