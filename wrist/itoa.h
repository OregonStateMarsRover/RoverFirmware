#ifndef CUSTOM_ITOA
#define CUSTOM_ITOA

#include <stdint.h>

/**
 * C++ version 0.4 char* style "itoa":
 * Written by Lukás Chmela
 * Released under GPLv3.
 */
uint8_t mitoa(int value, char* result, uint8_t base);

#endif
