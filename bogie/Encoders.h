/*
 * Encoders.h
 *
 * Created: 4/24/2012 4:18:42 PM
 *  Author: mfortner
 */ 

#ifndef BOGIE_ENCODERS
#define BOGIE_ENCODERS

#include "BogieCommon.h"

#define ACTUATOR_QUADRATURE_LINECOUNT 360U
#define MAGNETIC_ENCODER_SCALER 2000


// global variables
volatile int8_t measured_wheel_direction;
volatile uint8_t wheel_encoder_position;


void encoders_init( void );


//turn encoder functions
void turn_enc_init( void );
int16_t get_turn( void );

//drive encoder functions
void wheel_enc_init( void );
int16_t get_speed( void );

#endif
