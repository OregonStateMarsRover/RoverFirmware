/*BogieCommon.h
 *Authors: Mike Fortner, Chris Newman
 */

#ifndef BOGIE_COMMON_HEADER
#define BOGIE_COMMON_HEADER


/* Pull LEDs low to turn them on */
#define GREEN 0x10	// green LED on port D
#define RED 0x20	// red LED on port D

/* Limits are pulled up by default.
 * If they are low, that means it is okay */
#define LIM0 0x04	// limit 0 on port B
#define LIM1 0x08	// limit 1 on port B


#define ENCOA 0x10	// wheel encoder on PORT C
#define ENCOB 0x20	// wheel encoder on PORT C


#include "avr_compiler.h"

#include "USART.h"
#include "Sabertooth.h"
#include "SerialProtocol.h"
#include "Encoders.h"
#include "pid.h"

struct BogieControllerData {
	struct USART motor;
	struct USART bb;
	
	SerialData packet;
};

struct BogieControllerData bogie;

// Used for setting the system clock to 16Mhz
void set_clock( void );

#endif
