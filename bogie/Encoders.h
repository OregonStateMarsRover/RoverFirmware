/*
 * Encoders.h
 *
 * Created: 4/24/2012 4:18:42 PM
 *  Author: mfortner
 */ 

#ifndef BOGIE_ENCODERS
#define BOGIE_ENCODERS

#include "BogieCommon.h"
#include "qdec_driver.h"
#include "TC_driver.h"

#define ACTUATOR_QUADRATURE_LINECOUNT 360

#define DRIVE_ENCODER_LINECOUNT 1

int16_t get_speed( void );

int16_t get_turn(void);

void encoders_init( void );

#endif
