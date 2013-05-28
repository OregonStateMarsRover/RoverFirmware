/* * Wrist board
 * Author: Marshal Horn
 */

#include "wrist.h"

#define BUF_SIZE 50

int main( void ) {

	init();

	while( 1 ) {
	}


	return 0;
}

/* This is very sloppy of me - I don't check to see if the data sent is long
 * enough to decode. However, with the changes to the serial protocol, this is
 * more difficult. I don't have time to fix it right now, but it's important for
 * the robustness of this function.
 */
void handle_packet( SerialData * s ) {
	if( (s->receive_address == ADDRESS) && (s->receive_length > 0) ){
		uint16_t angle;
		switch( s->receive_data[0] ) {
			case 1:	// Wrist actuator
				angle = s->receive_data[1] | ((uint16_t)s->receive_data[2] << 8);
				if( angle > WRIST_MAX )
					angle = WRIST_MAX;
				else if( angle < WRIST_MIN )
					angle = WRIST_MIN;
				PanTiltSetPosition( WRIST_SERVO, angle );
				break;

			case 2: // Tilt servo
				/* Not sure what this is right now, so I'm leaving it alone.
				 * It's probably the second attachement, for cleaning off
				 * the solar cell.
				 */
				break;

			case 3: // Scoop
				angle = s->receive_data[1] | ((uint16_t)s->receive_data[2] << 8);
				PanTiltSetPosition( SCOOP_SERVO, angle );
				break;

			case 4: // Probe actuate
				angle = s->receive_data[1] | ((uint16_t)s->receive_data[2] << 8);
				PanTiltSetPosition( PROBE_SERVO, angle );
				break;

			case 5:	// Take probe measurement
				break;

			case 6:	// Measure voltage
				break;

			default: // Do nothing
				break;
		}
	}
}

void test_ax12( void ) {
	PanTiltSetPosition( SCOOP_SERVO, 0 );
	PORTE.OUTTGL = 0x01;
	_delay_ms( 2000 );
	PanTiltSetPosition( SCOOP_SERVO, 0x3FF );
	PORTE.OUTTGL = 0x01;
	_delay_ms( 2000 );


}

void init( void ) {
	set_clock();

	PORTE.DIRSET = 0x03;	// for status LEDs

	USART_InitPortStructs();
	wrist.bus 	=	USART_Open( 0, USART_BAUD_115200, 	20, 20, true );
	wrist.probe =	USART_Open( 1, USART_BAUD_9600, 	20, 20, true );
	wrist.ax12	=	USART_Open( 2, USART_BAUD_250000,		20, 20, true );
	wrist.aux	=	USART_Open( 3, USART_BAUD_115200, 	100, 20, false );

	char clear[9] = { 27, '[', '2', 'J', 27, '[', '?', '6', 'h' };	// for clearing the screen in a vt100 terminal

	USART_Write( wrist.aux, clear, 9 );
	USART_Write( wrist.aux, "Debugging port:\r\n", 17 );

	SerialDataInitialize( wrist.packet );
	wrist.packet->ReceivePacketComplete = handle_packet;
}

void test_probe( void ) {
	RingBuffer *probe_rx = &(wrist.probe->rx_buffer);

	char buf[BUF_SIZE];

	char num[12];

	enum probe_state{ init = 0, query, wait, process, idle };

	char * ind[5];
	ind[0] = "init";
	ind[1] = "query";
	ind[2] = "wait";
	ind[3] = "process";
	ind[4] = "idle";

	enum probe_state state = init;

	uint8_t rcv_len = 0;

	while( 1 ) {

		char * cur = ind[state];
		uint8_t len = strlen( cur );
		USART_Write( wrist.aux, "\r\nState: ", 9 );
		USART_Write( wrist.aux, cur, len );

		switch( state ) {
			case init:
				wrist.new_probe_data = 0;
				state = idle;
				break;

			case query:
				hydraprobe_query( "SN" );
				state = wait;
				break;

			case wait:
				if( wrist.new_probe_data ){
					state = process;
				} else {
					_delay_ms( 200 );
				}
				break;

			case process:

				rcv_len = RingBufferGetData( probe_rx, buf, BUF_SIZE );
				/* rcv_len = RingBufferBytesUsed( probe_rx );
				for( int i = 0; i <= rcv_len; ++i ) {
					buf[i] = RingBufferGetByte( probe_rx );
				} */
				USART_Write( wrist.aux, "\r\nReceived ", 11 );
				USART_Write( wrist.aux, num , mitoa( rcv_len, num, 10 ) );
				USART_Write( wrist.aux, " bytes: ", 8 );

				USART_Write( wrist.aux, buf, rcv_len);
				state = idle;
				wrist.new_probe_data = 0;
				USART_Rx_Enable( &USARTC1 );
				break;

			case idle:
				_delay_ms( 1000 );
				state = query;
				break;

			default:
				USART_Write( wrist.aux, "\r\ndefault case", 14 );
				break;

		}
		
	}
}
