/*BogieCommon.c
 *Authors: Marshal Horn
 */

#include "common.h"


/* This function sets the clock speed for the microcontroller.
 * When you change it, make sure to change F_CPU to match.
 * Currently, it changes the main clock to the external crystal,
 * and makes the prescaler 1.
 * However, the 9600 baud setting in UART.c is probably still set
 * for the 2Mhz clock, and also needs to be adjusted.
 */
void set_clock( void ) {
	// Wait until the oscilator is stable
	PORTE.DIRSET = 0x20;
	uint8_t i = 0;
	uint8_t osc_ok = 1;
	while( !(OSC.STATUS & 0x08) ) {
		PORTE.OUTSET = 0x20;	// turn off LED
		OSC.XOSCCTRL = 0b11001011;	// set up oscillator for 16 Mhz
		OSC.CTRL |= 0x08;	// Start the external clock.
		_delay_ms( 7 );
		PORTE.OUTCLR= 0x20;	// turn on LED
		_delay_ms(10);
		++i;
		if( i > 4 ) {
			osc_ok = 0;
			break;
		}
	}
	if( osc_ok ) {
		PORTE.OUTSET = 0x20;	// turn off red LED
		OSC.PLLCTRL = 0b11000001;	//Set PLL to use external clock
		
		CCP = 0xD8;	// enable change of protected registers
		CLK.CTRL = 3;		// use external clock

		CCP = 0xD8;	// enable change of protected registers
		CLK.PSCTRL = 0;		// disable prescaler
	} else {
		/* External crystal isn't working, so we'll use the internal
		 * 32Mhz oscillator instead.  Make sure to prescale it down
		 * to 16Mhz so our UART baud rates aren't off.
		 */
		i = 0;
		while( !(OSC.STATUS & 0x02) ) {
			PORTE.OUTSET = 0x20;	// turn off LED
			OSC.CTRL |= 0x02;	// Start the 32Mhz oscillator
			_delay_ms( 7 );
			PORTE.OUTCLR= 0x20;	// turn on LED
			_delay_ms(15);
			++i;
		}
		PORTE.OUTSET = 0x20;	// turn off red LED
		
		CCP = 0xD8;	// enable change of protected registers
		CLK.CTRL = 1;		// use 32Mhz oscillator

		CCP = 0xD8;	// enable change of protected registers
		CLK.PSCTRL = 1;		// divide clock by 2
	}
}


/* Setup the realtime clock.
 * Use a period in miliseconds.
 */
void setup_rtc( uint8_t prescaler ) {
	/* Enable the ULP clock */
	CLK.RTCCTRL = 0x01;

	/* Setup the RTC */
	RTC.CNT = 0;
	RTC.INTCTRL = 0;	// no interrupt
	RTC.CTRL = prescaler & 0x07;
}



/* Basic packet handler to allow me to test the RS485 communication.
 * This will probably get its own file later, but for now, we're using it
 * as the serial callback.
 *
void handle_packet( SerialData * s ) {
	if( s->receive_address == ADDRESS ) {
		PORTE.OUTTGL = GREEN;
		switch (s->receive_data[0] ) {
			int16_t angle;	// dont care about initial value
			case 1:
				// Update shoulder angle
				 angle = s->receive_data[1] | (s->receive_data[2]<<8);
				if( angle > 144 )
					angle = 144;
				else if( angle < -9 )
					angle = -9;
				arm.shoulder.setpoint = angle;
				break;
			case 2:
				// Update elbow angle
				angle = s->receive_data[1] | (s->receive_data[2]<<8);
				if( angle > 203 )
					angle = 203;
				else if( angle < 4 )
					angle = 4;
				arm.elbow.setpoint = angle;
				break;
			default:
				// Do nothing?
				break;
		}

	}
}
*/


/* Receive data error.
 * Send error back over RS485
 *
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

*/
