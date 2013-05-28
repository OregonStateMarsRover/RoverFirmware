#ifndef WRIST_HEADER
#define WRIST_HEADER

/* Insteaad of including all headers in common,
 * and having all headers include common,
 * I'm just going to put the actual common stuff
 * in common (like global structs), and include
 * everything in this header instead. This header
 * is only included by wrist.c.
 */

#include "common.h"
#include "USART.h"
#include "itoa.h"

#include "hydraprobe.h"
#include <string.h>

void init( void );


#endif
