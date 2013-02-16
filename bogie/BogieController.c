/*BogieController.c
 *Authors: Marshal Horn, Mike Fortner, Chris Newman
 */

#include "BogieController.h"
#include <string.h>
#include <stdio.h>

#define BOGIE_ADDRESS 7 // Address of this unique bogie controller


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
	USART_Open(&bogie.bb, 0, USART_BAUD_115200, 100, 10, true, false);
	
	SerialDataInitialize( &bogie.packet );
	// Set behavior when packet is received
	bogie.packet.ReceivePacketComplete = handle_packet;
	// Return error over RS485
	bogie.packet.ReceiveDataError = packet_error;

	/*** Initialize Sabertooth Motor Driver ***/
	
	sabertooth_init(&bogie.motor);
	encoders_init();

	setup_rtc( 10 );
	/* void pid_setup( struct pid * settings, int16_t p, int16_t i, int16_t d, int16_t ramp, uint8_t dt) */
	pid_setup( &speed_pid, 20, 0, 0, 300, 10 );

	
	
	
	sei();
}


/* Basic packet handler to allow me to test the RS485 communication.
 * This will probably get its own file later, but for now, we're using it
 * as the serial callback.
 */
void handle_packet( SerialData * s ) {
	if( s->receive_address == BOGIE_ADDRESS 
			/*&& s->receive_length >= 3*/ ) {
		bogie.drive = s->receive_data[0];
		bogie.turn = s->receive_data[1];
	}
}


/* Receive data error.
 * Send error back over RS485
 */
void packet_error( SerialData *s, uint8_t errCode ) {
	char * msg;
	unsigned short len = 0;
	switch( errCode ){
		case ERR_START_BYTE_INSIDE_PACKET:
			msg = "ERR: startbyte inside packet\r\n";
			break;
		case ERR_EXPECTED_START_BYTE:
			msg = "ERR: no start\r\n";
			break;
		case ERR_UNKNOWN_ESCAPED_CHARACTER:
			msg = "ERR: unknow escape\r\n";
			break;
		case ERR_EXCESSIVE_PACKET_LENGTH:
			msg = "ERR: packet too long\r\n";
			break;
		case ERR_CHECKSUM_MISMATCH:
			msg = "ERR: bad checksum\r\n";
			break;
		case ERR_BUFFER_INSUFFICIENT:
			msg = "ERR: buffer too short\r\n";
			break;
		case ERR_RECEIVED_IGNORE_BYTE:
			msg = "ERR: received bad byte\r\n";
			break;
		default:
			msg = "ERR: unkown\r\n";
			break;
	}
	len = strlen(msg);

	USART_Write( &bogie.bb, (uint8_t *)msg, len );
}



int main(void)
{

	init();
	/*
	char msg[50];
	unsigned short len = 0;
	*/
	
	RingBuffer * buffer = &(bogie.bb.rx_buffer);
	uint8_t new_data;
	uint8_t prescaler = 0;

	USART_Write( &(bogie.bb), (uint8_t *)"Welcome!\r\n", 10 );

	while(1) {
		++prescaler;

		if( RingBufferBytesUsed( buffer ) ) {
			//PORTD.OUTTGL = GREEN;
			new_data = RingBufferGetByte( buffer );
			ProcessDataChar( &(bogie.packet), new_data );
		} else {
			_delay_ms( 3 );
		}
		if( prescaler % 32  == 0 ) {
			print_pid( &turn_pid );
		}
	}


	return 0;
}
