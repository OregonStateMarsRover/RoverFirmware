/*arm.c
 *Authors: Marshal Horn
 */

#include "arm.h"

#define ADDRESS 8 // Address of the arm controller

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
		
	
	USART_Open(&arm.motor, 2, USART_BAUD_9600, 10, 10, false);
	/***Mainboard USART init***/
	USART_Open(&arm.bb, 0, USART_BAUD_115200, 100, 100, true);
	
	SerialDataInitialize( &arm.packet );
	// Set behavior when packet is received
	arm.packet.ReceivePacketComplete = handle_packet;
	// Return error over RS485
	//arm.packet.ReceiveDataError = packet_error;

	/*** Initialize Sabertooth Motor Driver ***/
	
	sabertooth_init(&arm.motor);
	PORTD.OUTSET = 0x04;	// enable motor
	
	
	sei();
}


/* Basic packet handler to allow me to test the RS485 communication.
 * This will probably get its own file later, but for now, we're using it
 * as the serial callback.
 */
void handle_packet( SerialData * s ) {
	if( s->receive_address == ADDRESS ) {
		PORTD.OUTTGL = RED;	// toggle red LED
		// No actions yet

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
	USART_WriteByte( &arm.bb, msg );
}


struct pid{
	int16_t setpoint;
	int16_t pv_scale;	// treat as 8.8 fixed point
	int16_t pv_offset;
	int16_t pv;			// calculated, not raw
	int16_t p;			// treat as 8.8 fixed point
	int8_t output;
};

void update_pid( struct pid v, uint8_t enc ) {
	v.pv = get_angle(enc) * v.pv_scale - v.pv_offset;
	//v.ouput = (v.setpoint - v.pv) * v.p;
}


int main(void)
{

	init();

	RingBuffer * buffer = &(arm.bb.rx_buffer);
	uint8_t new_byte;
	uint16_t time = get_time();
	int8_t test_speed = 15;

	struct pid shoulder, elbow;
	shoulder.pv_scale = 0x0100;
	shoulder.pv_offset = 0;
	shoulder.p = 0;
	shoulder.output = test_speed;

	elbow.pv_scale = 0x0100;
	elbow.pv_offset = 0;
	elbow.p = 0;
	elbow.output = test_speed;

	while(1) {
		if( get_time() - time > 100 ) 
		{

			time = get_time();
			
			update_pid( shoulder, 0 );
			update_pid( elbow, 1 );


			uint8_t limits = PORTB.IN;

			/* Test the motors and limit directions */
			if( (limits & LIM0) && (limits & LIM1) )
				shoulder.output = 0;
			else if( limits & LIM0 )
				shoulder.output = -test_speed;
			else if( limits & LIM1 )
				shoulder.output = test_speed;

			if( (limits & LIM2) && (limits & LIM3) )
				elbow.output = 0;
			else if( limits & LIM2 )
				elbow.output = -test_speed;
			else if( limits & LIM3 )
				elbow.output = test_speed;
			
			elbow_set( elbow.output );
			shoulder_set( shoulder.output );

		}

		else if( RingBufferBytesUsed( buffer ) ) 
		{
			PORTD.OUTTGL = GREEN;
			
			new_byte = RingBufferGetByte( buffer );
			ProcessDataChar( &(arm.packet), new_byte );
		} 
	}


	return 0;
}

