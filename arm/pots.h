#ifndef DIGITAL_SPI_MAGNETIC_POTENTIOMETER
#define DIGITAL_SPI_MAGNETIC_POTENTIOMETER

#include "common.h"


void init_spi( uint8_t prescaler );


/* Main function, for getting an angle.
 * Will stop the code until it's received.
 * I'll change this over to an interrupt-based method if it takes too long
 */
int16_t get_angle( uint8_t channel );


#endif
