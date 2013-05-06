/*
 * Specs for the hydraprobe:
 * 9600 baud rate
 * 81N
 *
 * End commands with \r\n
 *
 * Half-duplex
 *
 * The address is currently "mar"
 *
 * Communication protocol on pg 48
 */

#include "hydraprobe.h"


uint8_t hydraprobe_exec( char * address, char * command );
	USART_Write( &wrist.soil, address, 3 );
	USART_Write( &wrist.soil, command, 2 );
	USART_Write( &wrist.soil, "\r\n", 2 );

	return 1;
}


/* The way I see this working is having a bunch of functions that
 * do different things - one for setting parameters, one for
 * getting parameters, one for parsing regular replies, one for
 * taking a reading (and asking for the measurement back), and
 * one for parsing the measurement back.
 * I figure that this will operate on a state machine in main(), 
 * but the interrupt can catch when the receive message is complete
 * (When a newline character is received), and set a flag.
 * Since it is half-duplex, the send/receive state should be configured
 * in the state machine (so as not to prevent other interrupts).
 * The interrupt should also keep track of the length of the message,
 * so when the full message is received, it can be memcpy'd out of the
 * buffer in order to be parsed.
 */
