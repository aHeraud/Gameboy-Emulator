#ifndef REGISTER16_H
#define REGISTER16_H

#include<stdint.h>

/**
	Add the value of 2 16 bit registers together
	Flags: - 0 H C
*/
void add16(uint16_t *r, uint16_t other, uint8_t *flags);

/**
	Add/Subtract a signed byte to/from a 16 bit register
	Flags: 0 0 H C
*/
void add_sp_nn(uint16_t *r, uint8_t other, uint8_t *flags);

#endif