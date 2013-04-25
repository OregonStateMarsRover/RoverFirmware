#include "servo.h"


uint8_t servos_init( void ){
	/*
	 * Init the USART
	 *
	 * Check that both servos are fine.  (Correct ID, no error, etc.)
	 *
	 * Configure them for any special settings.
	 *
	 * Enable the outputs.
	 *
	 * Return any errors
	 */
}


/* This is an example of how to write a servo function.
 * It's not terribly effecient, but it's short and fairly easy to read.
 */
uint8_t servo_set_angle( struct servo * s, uint16_t angle ) {
	uint8_t data[2] = { angle, angle >> 8 };
	struct message m = { 30, &data, 2 }; // A "set position" message

	return servo_write_data( s, &m );
}
