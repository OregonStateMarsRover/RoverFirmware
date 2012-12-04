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

#define ACTUATOR_QUADRATURE_LINECOUNT 2U

//drive encoder functions

uint16_t get_drive_speed( void );

//actuator encoder functions

void actuator_init( void );

uint16_t get_actuator_pos( void );

void encoders_init( void );

void wheel_enc_init( void );

int16_t wheel_speed( void );

#endif
