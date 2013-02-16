/*BogieController.h
 *Authors: Mike Fortner, Chris Newman
 */

#ifndef BOGIE_CONTROLLER_H
#define BOGIE_CONTROLLER_H

#include "avr_compiler.h"
#include "BogieCommon.h"

void handle_packet( SerialData * s );

void packet_error( SerialData *s, uint8_t errCode );

void init(void);



#endif
