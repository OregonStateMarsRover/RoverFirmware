/*BogieController.c
 *Authors: Marshal Horn, Mike Fortner, Chris Newman
 */

#include "BogieController.h"

#define GREEN 0x10	// green LED on port D
#define RED 0x20	// red LED on port D
#define LIM0 0x04	// limit 0 on port B
#define LIM1 0x08	// limit 1 on port B


void init(void)
{
	USART_InitPortStructs();
	
	/***set I/O port directions***/
	PORTA.DIR = 0x00;
	PORTB.DIR = 0x00;
	PORTC.DIR = 0b00001010;	//only outputs are UART TX
	PORTD.DIR = 0b00111110;	//all outputs; rest are unused

	/***Motor Driver USART init***/
		
	
	USART_Open(&bogie.motor, 2, USART_BAUD_9600, 10, 10, false, false);
	/***Mainboard USART init***/
	/*
	USART_Open(&bogie.bb, 0, USART_BAUD_38400, 10, 10, false, true);
	CommInterfaceInit(&bogie.bb_com, &bogie.bb);
	
	PacketQueueInit2(&bogie.pktQueue, 6, 20, bogie.queuedPackets, bogie.queuedData);
	*/



	/*** Initialize Sabertooth Motor Driver ***/
	
	sabertooth_init(&bogie.motor);
	
	
	sei();
}



int main(void)
{
	init();

	/* Currently testing LEDs and limit switches.
	 * The motor controller wasn't acting how I thought it should, but it may be
	 * how I'm writing to the LEDs, because they seemed funny as well.
	 * There may be more effecient ways of doing this (a bit-shift, for example),
	 * but for now I'm going to treat each limit seperately, and use the OUTSET
	 * and OUTCLR registers, since that's what I was doing before.
	 *
	 * The LEDs should mimic the states of the limit switches.  So, if both 
	 * switches are disconnected (or pressed), then both LEDs should be on.
	 * This also serves as a test of whether the limit switches are normally-
	 * closed or normally-open.  They should be normally-closed, so that if 
	 * they get disconnected, they will read as pressed, and the motor will
	 * not move.  (In fact, the motor controller can notice that and tell the
	 * beaglebone that the limit switches are disconnected)
	 */
	while(1) {
		drive_set( i );
		PORTD.OUTTGL = GREEN;


		if( ~PORTB.IN & (LIM0 | LIM1) ) {
			actuator_set( 0 );
			PORTD.OUTSET = RED;
		} else {
			actuator_set( i );	// Be careful with this until we have limits set up
			PORTD.OUTCLR = RED;
		}

		_delay_ms( 50 );

		if( i == 127 ) {
			inc = -1;
		} else if( i == -127 ) {
			inc = 1;
	}

	return 0;
}
