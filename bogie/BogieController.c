/*BogieController.c
 *Authors: Marshal Horn, Mike Fortner, Chris Newman
 */

#include "BogieController.h"
#include <string.h>

#define BOGIE_ADDRESS 3 // Address of this unique bogie controller


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
	//bogie.packet.ReceiveDataError = packet_error;

	/*** Initialize Sabertooth Motor Driver ***/
	
	sabertooth_init();
	PORTD.OUTSET = 0x04;	// enable motor
	
	
	sei();
}


/* Basic packet handler to allow me to test the RS485 communication.
 * This will probably get its own file later, but for now, we're using it
 * as the serial callback.
 */
void handle_packet( SerialData * s ) {
	if( s->receive_address == BOGIE_ADDRESS ) {
		bogie.drive = s->receive_data[0];
		bogie.turn = s->receive_data[1];
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

	RingBuffer * buffer = &(bogie.bb.rx_buffer);
	uint8_t new_data;
	uint8_t prescaler = 0;

	USART_Write( &(bogie.bb), (uint8_t *)"Welcome!\r\n", 10 );

	while(1) {
		++prescaler;

		if( RingBufferBytesUsed( buffer ) ) {
			new_data = RingBufferGetByte( buffer );
			ProcessDataChar( &(bogie.packet), new_data );
		} else {
			_delay_ms( 20 );
		}
		if( prescaler % 32 == 0 ) {
			print_pid( &turn_pid );
		}
	}


	return 0;
}

