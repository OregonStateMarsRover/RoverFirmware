/*BogieController.c
 *Authors: Marshal Horn, Mike Fortner, Chris Newman
 */

#include "BogieController.h"

/* Pull LEDs low to turn them on */
#define GREEN 0x10	// green LED on port D
#define RED 0x20	// red LED on port D

/* Limits are pulled up by default.
 * If they are low, that means it is okay */
#define LIM0 0x04	// limit 0 on port B
#define LIM1 0x08	// limit 1 on port B


void init(void)
{
	set_clock( ); // set clock t 16Mhz

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

	int i = 0;

	int inc = 1;

	while(1) {
		drive_set( i );
		i += inc;
		PORTD.OUTTGL = GREEN;


		/*
		if( PORTB.IN & (LIM0 | LIM1) ) {
			actuator_set( 0 );
			PORTD.OUTCLR = RED;
		} else {
			actuator_set( i );	// Be careful with this until we have limits set up
			PORTD.OUTSET = RED;
		}
		*/

		_delay_ms( 500 );

		if( i == 127 ) {
			inc = -1;
		} else if( i == -127 ) {
			inc = 1;
		}
	}

	return 0;
}
