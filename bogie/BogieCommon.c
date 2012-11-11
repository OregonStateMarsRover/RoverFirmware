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
	// Wait until the oscilator is stable
	PORTD.DIRSET = 0x20;
	while( !(OSC.STATUS & 0x40) ) {
		PORTD.OUTTGL = 0x20;	// toggle red LED
		OSC.XOSCCTRL = 0b11011011;	// set up oscillator for 16 Mhz
		OSC.CTRL |= 0x40;	// Start the external clock.
		_delay_ms(50);
	}
	PORTD.OUTCLR = 0x20;	// turn off red LED
	//OSC.PLLCTRL = 0b11000001;	//Set PLL to use external clock
	
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

/* This needs the encoders to be functional before it will work.
 * The higher priority for encoders is the quadrature encoder for tracking wheel turn, because
 * the wheel rotation is always commanded in terms of position, not speed.
 */
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


/* How can we make this run every time a packet is received?
 * Of course, we should only pay attention to packets that are sent too this controller.
 * I believe the packet handlers will accept packets addressed to anyone, and it's our
 * job to make sure it works out.
 * How are we assigning unique IDs to each one, anyways?  Maybe there should be a special
 * boot mode, or a command sent over RS485.  I think the limit switches and the encoders
 * are the closest thing to physical inputs we have.  (Too bad there's no DIP switches
 * on the board)
 */
void parse_command(CommPacket *pkt)
{
	// Should be changed to speed PID
	drive_set((int8_t)pkt->data[0]);

	// Should be changed to position PID
	actuator_set((int8_t)pkt->data[1]);
}

/* This hasn't been decided yet.
 * Some think there should be an ACK after a message is received - I think that should
 * be taken care of in a lower-level protocol (rs485 is full-duplex, after all).  However,
 * the best plan for now is to stick with the already existing (and tested) code.
 */
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
