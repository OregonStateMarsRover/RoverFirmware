#ifndef WRIST_HYDRAPROBE
#define WRIST_HYDRAPROBE

#include "common.h"
#include "USART.h"

uint8_t hydraprobe_exec( char * command );

uint8_t hydraprobe_query( char * command );

uint8_t hydraprobe_assign( char * command, char * data, uint8_t len );

uint8_t hydraprobe_measure( uint8_t set );

char * hydraprobe_index( char * message, uint8_t index );

char * hydraprobe_copy( char * dest, char * message );

uint8_t hydraprobe_decode( char * dest, char * message );


#endif
