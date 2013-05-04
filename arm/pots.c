/*
 * Interface for the digital potentiometers.
 * They use a SPI bus, but it appears to actually be 12 bit 
 *
 * Author: Marshal Horn
 */

#include "pots.h"


// Clock should not be faster than 4Mhz, so we should pass a prescaler of at least 1.
void init_spi( uint8_t prescaler ) {
	// Make prescaler range from 0 to 8
	prescaler = ((prescaler & 0x06)>>1) 	// regular prescaler
		| ((~prescaler & 0x01)<<8);			// CLK2X bit

	// Enable, master, MSB first, mode 2
	SPIC.CTRL = prescaler | 0x58;

}


/* Main function, for getting an angle.
 * Will stop the code until it's received.
 * I'll change this over to an interrupt-based method if it takes too long
 */
int16_t get_angle( uint8_t channel ) {
	int16_t angle;
	// Set the slave select bit
	if( channel )
		PORTC.OUTSET = 0x10;
	else
		PORTC.OUTCLR = 0x10;

	//Receive byte 1
	SPIC.DATA = 0;	//data isn't sent anyway
	while(!(SPIC.STATUS & 0x80 ));
	angle = SPIC.DATA<<4;
	
	//Receive byte 2
	SPIC.DATA = 0;	//data isn't sent anyway
	while(!(SPIC.STATUS & 0x80 ));
	angle |= SPIC.DATA>>4;
	
	return angle;
}

