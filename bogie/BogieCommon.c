/*BogieCommon.c
 *Authors: Mike Fortner, Chris Newman
 */

#include "BogieCommon.h"

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
		

	encoders_init();
	
	TC0_t *loop_timer = &TCD0;
	TC_SetPeriod( loop_timer, 195U); //set period to (2000000/1024)(ticks/sec)/10(loops/sec) = 195 ticks/loop
	TC0_ConfigClockSource( loop_timer,  TC_CLKSEL_DIV1024_gc);  //set TCD0 to count the system clock. frequency should be 2,000,000 ticks/sec
	TC0_SetOverflowIntLevel( loop_timer, TC_OVFINTLVL_LO_gc);  //set TCD0 to trigger an interrupt when every overflow (100ms)

}

/*
ISR(TCD0_OVF_vect)
{
 	PORTD.OUTTGL = 0b00110000;
 	drive_set(desired_speed); 
 	actuator_set(pid(&act_pid, desired_angle, get_actuator_pos()));
}
*/


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
/*
ISR(TCD0_OVF_vect)
{
 	PORTD.OUTTGL = 0b00110000;
 	drive_set(desired_speed); 
 	actuator_set(pid(&act_pid, desired_angle, get_actuator_pos()));
}
*/
