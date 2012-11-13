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
	set_clock( ); // set clock to 16Mhz

	USART_InitPortStructs();
	
	/***set I/O port directions***/
	PORTA.DIR = 0x00;
	PORTB.DIR = 0x00;
	PORTC.DIR = 0b00001010;	//only outputs are UART TX
	PORTD.DIR = 0b00111110;	//all outputs; rest are unused

	/***Motor Driver USART init***/
		
	
	USART_Open(&bogie.motor, 2, USART_BAUD_9600, 10, 10, false, false);
	/***Mainboard USART init***/
	USART_Open(&bogie.bb, 0, USART_BAUD_115200, 10, 10, true, false);
	
	SerialDataInitialize( &bogie.packet );
	// Set behavior when packet is received
	bogie.packet.ReceivePacketComplete = handle_packet;

	/*** Initialize Sabertooth Motor Driver ***/
	
	sabertooth_init(&bogie.motor);
	
	
	sei();
}


/* Basic packet handler to allow me to test the RS485 communication.
 * This will probably get its own file later
 */
void handle_packet( SerialData * s ) {
	bogie_drive = s->receive_data[0];
	bogie_turn = s->receive_data[1];
	PORTD.OUTTGL = 0x20;	// toggle red LED
}





int main(void)
{

	init();

	while(1) {

		drive_set( bogie_drive );
		actuator_set( bogie_turn );
		RingBuffer * buffer = &(bogie.bb.rx_buffer);

		if( RingBufferBytesUsed( buffer ) ) {
			PORTD.OUTTGL = GREEN;
			ProcessDataChar( &(bogie.packet), RingBufferGetByte( buffer ) );
		}


		_delay_ms( 50 );
	}
		

	return 0;
}
