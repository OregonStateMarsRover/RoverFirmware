/*BogieController.c
 *Authors: Marshal Horn, Mike Fortner, Chris Newman
 */

#include "BogieController.h"
#include <stdio.h>
#include <string.h>

#define BOGIE_ADDRESS 5 // Address of this unique bogie controller
//#define MOTOR_TEST
//#define PACKET_DEBUG
//#define RS485_TEST

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
#ifdef PACKET_DEBUG
	bogie.packet.ReceiveDataError = packet_error;
#endif

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
		bogie_drive = s->receive_data[0];
		//bogie_turn = s->receive_data[1];
		drive_set( bogie_drive );
#ifdef PACKET_DEBUG
		PORTD.OUTTGL = RED;	// toggle red LED
#endif

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
	uint8_t new_byte;
	
#ifdef MOTOR_TEST
	int8_t i = 0;	int8_t dir = 1;
	while(1) {
		drive_set( i );
		i += dir;
		if( (i == 127 ) || (i == -127) ) {
			dir = -dir;
		}
		_delay_ms(20);
	}
#endif

#ifdef RS485_TEST
	while(1) {
		USART_Write(&bogie.bb, "Hello World!\n\r", 14 );
		_delay_ms( 500 );
	}
#endif

	USART_Write( &bogie.bb, (uint8_t *)"Encoder test\r\n", 14 );

	int8_t i = 0;
	int8_t inc = 1;
	uint8_t flash_mode;

	char msg[20];
	uint8_t msg_len;

	/* Ramp the drive motor speed up and down,
	 * to test the motor controllers */
	while(1) {
		msg_len = snprintf( msg, 20, "%d, ", get_turn() );
		if( msg_len > 20 ) msg_len = 20;
		USART_Write( &bogie.bb, (uint8_t *)msg, msg_len );
			
		actuator_set( i );
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


/*
		if( RingBufferBytesUsed( buffer ) ) {
			PORTD.OUTTGL = GREEN;
			
			new_byte = RingBufferGetByte( buffer );
			ProcessDataChar( &(bogie.packet), new_byte );
		} else {
			_delay_ms( 10 );
		}
*/
	}

	return 0;
}
