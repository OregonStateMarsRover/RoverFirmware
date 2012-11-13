/*BogieController.h
 *Authors: Mike Fortner, Chris Newman
 */

#ifndef BOGIE_CONTROLLER_H
#define BOGIE_CONTROLLER_H

#include "avr_compiler.h"
#include "BogieCommon.h"

// Global variables for setting drive speed
signed char bogie_drive = 0;
signed char bogie_turn = 0;

void handle_packet( SerialData * s );


void init(void);



#endif
