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
#define LIM0 0x04	// left limit 0 on port B
#define LIM1 0x08	// right limit 1 on port B
#define LIMITS (PORTB.IN & 0x0C)

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

	encoders_init();

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


void zero_encoders( void ) {
	int8_t abs_speed = 100;

	while(!(LIMITS & LIM1)) {

		actuator_set( abs_speed );

		_delay_ms(5);
	}
	actuator_set( 0 );
	_delay_ms( 500 );
	TCC1.CNT = 0;

	char msg[20];
	uint8_t msg_len;

	pid_steer();

	msg_len = snprintf( msg, 20, "%d, ", bogie.steer.pv );
	if( msg_len > 20 ) msg_len = 20;
	USART_Write( &bogie.bb, (uint8_t *)msg, msg_len );

	while(!(LIMITS & LIM0) && (get_turn() < 20000 )) {

		actuator_set( -abs_speed );
		_delay_ms(5);
	}
	actuator_set( 0 );
	_delay_ms(5);
	
}



int main(void)
{
	init();
	
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
	int8_t abs_speed = 5;

	bogie.steer.pv_scale = 1;
	bogie.steer.p = 0;
	bogie.steer.output = 0;
	bogie.steer.setpoint = 120;

	//zero_encoders();

	char msg[20];
	uint8_t msg_len;
	int8_t i = 0;

	while(1) {
		uint8_t limits = PORTB.IN & 0x0C;

		if( limits == LIM0 )
			bogie.steer.output = abs_speed;
		else if( limits == LIM1 )
			bogie.steer.output = -abs_speed;
		else if( limits )
			bogie.steer.output = 0;

		actuator_set( bogie.steer.output );


		pid_steer();

		if( !(i % 8 ) ){
			i = 0;
			msg_len = snprintf( msg, 20, "%x %d\n\r", (PORTC.IN & 0xF0), bogie.steer.pv);
			if( msg_len > 20 ) msg_len = 20;
			USART_Write( &bogie.bb, (uint8_t *)msg, msg_len );
		}

		++i;

		_delay_ms( 50 );
	}

	return 0;
}
