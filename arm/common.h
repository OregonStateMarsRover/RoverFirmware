/*BogieCommon.h
 *Authors: Mike Fortner, Chris Newman
 */

#ifndef BOGIE_COMMON_HEADER
#define BOGIE_COMMON_HEADER

#include "avr_compiler.h"

#include "USART.h"
#include "Sabertooth.h"
#include "SerialProtocol.h"

// Limit switches are on PORTB
// If they are hi, that means they aare pressed or disconnected
#define LIM0 0x01
#define LIM1 0x02
#define LIM2 0x04
#define LIM3 0x08
#define LIMS 0x0F


/* Pull LEDs low to turn them on */
#define GREEN 0x10	// green LED on port D
#define RED 0x20	// red LED on port D

struct status_data{
	struct USART motor;
	struct USART bb;
	
	SerialData packet;
};

struct status_data arm;

// Used for setting the system clock to 16Mhz
void set_clock( void );

#endif
