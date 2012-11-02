/*BogieController.c
 *Authors: Marshal Horn, Mike Fortner, Chris Newman
 */

#include "BogieController.h"


void bogie_controller_init(void)
{
	USART_InitPortStructs();
	
	/***set I/O port directions***/
	PORTA.DIR = 0x00;
	PORTB.DIR = 0x00;
	PORTC.DIR = PIN1_bm | PIN3_bm;
	PORTD.DIR = PIN1_bm | PIN2_bm | PIN3_bm | PIN4_bm | PIN5_bm;

	/***Motor Driver USART init***/
		
	
	USART_Open(&bogie.motor, 2, USART_BAUD_9600, 10, 10, false, true);
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
	bogie_controller_init();

	int8_t i = 0;
	int8_t inc = 1;

	while(1) {
		drive_set( i );
		i += inc;
		_delay_ms( 10 );

		if( i == 127 ) {
			inc = -1;
		} else if( i == -127 ) {
			inc = 1;
		}
	}

	return 0;
}
