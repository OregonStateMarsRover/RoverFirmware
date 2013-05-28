/*
 * Wrist board
 * Author: Marshal Horn
 */

#include "wrist.h"

#define BUF_SIZE 50


int main( void ) {

	set_clock();

	PORTE.DIRSET = 0x03;	// for status LEDs

	USART_InitPortStructs();
	wrist.bus 	=	USART_Open( 0, USART_BAUD_115200, 	20, 20, true );
	wrist.probe =	USART_Open( 1, USART_BAUD_9600, 	20, 20, true );
	wrist.ax12	=	USART_Open( 2, USART_BAUD_1M,		20, 20, true );
	wrist.aux	=	USART_Open( 3, USART_BAUD_115200, 	100, 20, false );

	char clear[9] = { 27, '[', '2', 'J', 27, '[', '?', '6', 'h' };	// for clearing the screen in a vt100 terminal

	USART_Write( wrist.aux, clear, 9 );
	USART_Write( wrist.aux, "Debugging port:\r\n", 17 );

	struct RingBuffer *probe_rx = &((struct USART *)wrist.probe)->rx_buffer;

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

	return 0;
}

