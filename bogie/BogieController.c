/*BogieController.c
 *Authors: Marshal Horn, Mike Fortner, Chris Newman
 */

#include "BogieController.h"
#include <string.h>

#define BOGIE_ADDRESS 3 // Address of this unique bogie controller

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
		
	
	USART_Open(&bogie.motor, 2, USART_BAUD_9600, 10, 10, false);
	/***Mainboard USART init***/
	USART_Open(&bogie.bb, 0, USART_BAUD_115200, 100, 100, true);
	
	SerialDataInitialize( &bogie.packet );
	// Set behavior when packet is received
	bogie.packet.ReceivePacketComplete = handle_packet;
	// Return error over RS485
	bogie.packet.ReceiveDataError = packet_error;

	/*** Initialize Sabertooth Motor Driver ***/
	
	sabertooth_init(&bogie.motor);
	PORTD.OUTSET = 0x04;	// enable motor
	
	
	sei();
}


/* Basic packet handler to allow me to test the RS485 communication.
 * This will probably get its own file later, but for now, we're using it
 * as the serial callback.
 */
void handle_packet( SerialData * s ) {
	if( s->receive_address == BOGIE_ADDRESS ) {
		USART_WriteByte( &bogie.bb, (uint8_t)( s->receive_data[0] - bogie_drive + '5' ));
		USART_WriteByte( &bogie.bb, (uint8_t)' ');
		bogie_drive = s->receive_data[0];
		//bogie_turn = s->receive_data[1];
		drive_set( bogie_drive );
		//actuator_set( bogie_turn );
		//PORTD.OUTTGL = RED;	// toggle red LED

	}
}


/* Receive data error.
 * Send error back over RS485
 */
void packet_error( SerialData *s, uint8_t errCode ) {
	unsigned char msg;
	switch( errCode ){
		case ERR_START_BYTE_INSIDE_PACKET:
			msg = 'S';
			break;
		case ERR_EXPECTED_START_BYTE:
			msg = 's';
			break;

		case ERR_RECEIVED_IGNORE_BYTE:
			msg = 'G';
			break;
		case ERR_UNKNOWN_ESCAPED_CHARACTER:
			msg = 'g';
			break;

		case ERR_BUFFER_INSUFFICIENT:
			msg = 'B';
			break;
		case ERR_EXCESSIVE_PACKET_LENGTH:
			msg = 'b';
			break;

		case ERR_CHECKSUM_MISMATCH:
			msg = 'K';
			break;
		default:
			msg = '?';
			break;
	}
	USART_WriteByte( &bogie.bb, msg );
}



int main(void)
{

	init();
	USART_Write( &bogie.bb, (uint8_t *)"\n\rReset:\r\n", 10 );


	RingBuffer * buffer = &(bogie.bb.rx_buffer);
	uint8_t new_byte;

	while(1) {
		
		// Check if rcv buffer overflowed
		if( bogie.bb.rx_buffer.err ) {
			PORTD.OUTCLR = RED;
		} else {
			PORTD.OUTSET = RED;
		}


		if( RingBufferBytesUsed( buffer ) ) {
			PORTD.OUTTGL = GREEN;
			
			new_byte = RingBufferGetByte( buffer );
			ProcessDataChar( &(bogie.packet), new_byte );
		} else {
			_delay_ms( 10 );
		}
	}


	return 0;
}

