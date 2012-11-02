/*BogieController.c
 *Authors: Marshal Horn, Mike Fortner, Chris Newman
 */

#include "BogieController.h"

#define GREEN 0x10;	// green LED
#define RED 0x20;	// red LED


void init(void)
{
	USART_InitPortStructs();
	
	/***set I/O port directions***/
	PORTA.DIR = 0x00;
	PORTB.DIR = 0x00;
	PORTC.DIR = PIN1_bm | PIN3_bm;
	PORTD.DIR = PIN1_bm | PIN2_bm | PIN3_bm | PIN4_bm | PIN5_bm;

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

	int8_t i = 0;
	int8_t inc = 1;
	uint8_t flash_mode;

	/* Ramp the drive motor speed up and down,
	 * to test the motor controllers */
	while(1) {
		drive_set( i );
		// actuator_set( i );	// Be careful with this until we have limits set up
		_delay_ms( 40 );

		if( i == 127 ) {
			inc = -1;
		} else if( i == -127 ) {
			inc = 1;
		}
		i += inc;


		/* Flash the LEDs so we know what should be happening.
		 * ON means the motor is currently GOING that direction.
		 * BLINK means the motor is CHANGING TOWARDS that direction.
		 */
		flash_mode = (i & 0x80) | (inc & 0x01);
		switch( flash_mode ) {
				case 0x01:
						PORTD.OUTTGL = GREEN;
						PORTD.OUTCLR = RED;
						break;
				case 0x00:
						PORTD.OUTSET = GREEN;
						PORTD.OUTTGL = RED;
						break;
				case 0x80:
						PORTD.OUTCLR = GREEN;
						PORTD.OUTTGL = RED;
						break;
				case 0x81:
						PORTD.OUTTGL = GREEN;
						PORTD.OUTSET = RED;
						break;
		}




	}

	return 0;
}
