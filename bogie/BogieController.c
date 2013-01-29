/*BogieController.c
 *Authors: Marshal Horn, Mike Fortner, Chris Newman
 */

#include "BogieController.h"
#include <stdio.h>

#define GREEN 0x10;	// green LED
#define RED 0x20;	// red LED


void init(void)
{
	set_clock();

	USART_InitPortStructs();
	
	/***set I/O port directions***/
	PORTA.DIR = 0x00;
	PORTB.DIR = 0x00;
	PORTC.DIR = PIN1_bm | PIN3_bm;
	PORTD.DIR = PIN1_bm | PIN2_bm | PIN3_bm | PIN4_bm | PIN5_bm;

	/***Motor Driver USART init***/
		
	
	USART_Open(&bogie.motor, 2, USART_BAUD_9600, 10, 10, false, false);
	/***Mainboard USART init***/
	USART_Open(&bogie.bb, 0, USART_BAUD_115200, 50, 10, true, false );
	/*
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
	encoders_init();

	USART_Write( &bogie.bb, (uint8_t *)"Encoder test\r\n", 14 );

	int8_t i = 0;
	int8_t inc = 1;

	char msg[20];
	uint8_t msg_len;
	TCC1.CNT = 5;	// is this even changing?  Apparently not.

	/* Ramp the drive motor speed up and down,
	 * to test the motor controllers */
	while(1) {
		if( i % 1000 == 0 ) {
			msg_len = snprintf( msg, 20, "%d, ", get_turn() );
			if( msg_len > 20 ) msg_len = 20;
			USART_Write( &bogie.bb, (uint8_t *)msg, msg_len );

			actuator_set( 14 );
		} else {
			_delay_ms( 1 );
		}
		++i;

		// Manually generate events. Bit 0 of data controls direction
		uint8_t data = PORTC.IN >> 6;
		/*
		EVSYS.DATA = data >> 1;
		EVSYS.STROBE = data & 1;
		*/

		PORTD.OUT = data << 4;

		/*
		if( i == 127 ) {
			inc = -1;
		} else if( i == -127 ) {
			inc = 1;
		}
		i += inc;
		*/
	}
	return 0;
}
