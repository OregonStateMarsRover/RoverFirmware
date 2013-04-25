#include <util/delay.h>
#include <avr/io.h>

/* The datasheet for this board can be found in the h12 repository at
 * electrical/design/Tripod/PADS/tripod_sch.pdf
 */

int main( int argc, char **argv ) {
	/* Simple flash circuit */

	PORTA.OUTSET = 0x07;
	PORTA.OUTCLR = 0x07;
	

	while( 1 ) {
		PORTA.OUT++;
		_delay_ms( 500 );
	}

	return 0;
}
