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

#define PROBE "mar"
#define PORT wrist.probe

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

uint8_t hydraprobe_exec( char * command ){
	USART_Write( PORT, PROBE, 3 );
	USART_Write( PORT, command, 2 );
	USART_Write( PORT, "\r\n", 2 );
	return 0x01;
}

uint8_t hydraprobe_query( char * command ){
	USART_Write( PORT, PROBE, 3 );
	USART_Write( PORT, command, 2 );
	USART_Write( PORT, "=?\r\n", 4 );
	return 0x02;
}

uint8_t hydraprobe_assign( char * command, char * data, uint8_t len ){
	USART_Write( PORT, PROBE, 3 );
	USART_Write( PORT, command, 2 );
	USART_Write( PORT, "=", 1 );
	USART_Write( PORT, data, len );
	USART_Write( PORT, "\r\n", 2 );
	return 0x04;
}

uint8_t hydraprobe_measure( uint8_t set ){
	hydraprobe_exec( "TR" );
	char command[2];
	command[0] = 'T';
	command[1] = set + '0';
	hydraprobe_exec( command );
	return 0x08;
}

/* I think the receive buffer for this need not be a ring buffer;
 * A complete message is dealt with before another measurement is taken.
 * This means that the array doesn't need to be copied over, it can be dealt
 * with in-place, which should save operation time and memory.
 * The interrupt that handles the receiving on this UART can just set a flag
 * when a return is received, and then the decode function can start working.
 * There's no need to disable the receive, since the only remaining character
 * will occur after we care about it.  We don't even need to keep track of the
 * length of the array, since we'll just decode it as much as we want to, and
 * stop at the carriage return.
 */


/* This function is for indexing the message received;
 * It may not be effecient to convert the message to a number, but it's still
 * necessary to index the numbers.
 */
char * hydraprobe_index( char * message, uint8_t index ){
	if( !message )
		return 0;	// NULL in, NULL out

	uint8_t count = 0;
	while(*message) {
		if( (*message == '+') || (*message == '=' ) )
			++count;
		++message;
		if( count == index )
			break;
	}
	if( !(*message) )
		return 0;	// message not found
	else
		return message;
}

/* simple loop for extracting a value from the message.
 * Returns the new start value for destination
 *
 * WARNING:
 * This function uses pointer arithmatic and post-increments.
 * Read it very carefully.
 * When the probe sends us data, it starts with its address, and each subsequent
 * piece of data is preceeded by a '+' character. The command finishes with a
 * return and newline; since the return is first, we can just stop there.
 */
char * hydraprobe_copy( char * dest, char * message ) {
	while( *message != '+' && *message != '\r' ) {
		*dest++ = *message++;	// post-increment operators
	}
	*dest++ = ' ';	// space seperator
	return dest;
}

/* Assumes transmit set 2, and extracts degrees C, moisture, and conductivity.
 * Returns the number of characters in the string.
 *
 * This function assumes message contains data, and dest has been allocated with
 * plenty of room.
 */
uint8_t hydraprobe_decode( char * dest, char * message ) {
	char * orig = dest;
	char * tmp;
	tmp = hydraprobe_index( message, 1 ); 	// degrees C
	dest = hydraprobe_copy( dest, tmp );
	tmp = hydraprobe_index( message, 3 );	// moisture
	dest = hydraprobe_copy( dest, tmp );
	tmp = hydraprobe_index( message, 5 );	//electrical conductivity (temperature compensated)
	dest = hydraprobe_copy( dest, tmp );
	*dest = 0;	// NULL character

	return (dest - orig);	// size of string
}

#undef PROBE
#undef PORT
