/*
 * Encoders.h
 *
 * Created: 4/24/2012 4:18:42 PM
 *  Author: mfortner
 */ 

#include "BogieCommon.h"

#define ACTUATOR_QUADRATURE_LINECOUNT 2U

//drive encoder functions

uint16_t get_drive_speed( void );

//actuator encoder functions

void actuator_init( void );

uint16_t get_actuator_pos( void );
