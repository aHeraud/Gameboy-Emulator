#include"register16.h"
#include"register8.h"

void add16(uint16_t *r, uint16_t other, uint8_t *flags) {
	//Flags: - 0 H C
	uint16_t result = *r + other;
	*flags &= ZF_MASK;
	if (((*r & 0x0FFF) + (other & 0x0FFF)) > 0x0FFF) *flags |= HC_MASK;
	if (result < *r || result < other) *flags |= CF_MASK;
	*r = result;
}

void add_sp_nn(uint16_t *r, uint8_t other, uint8_t* flags) {
	//Flags: 0 0 H C
	//http://forums.nesdev.com/viewtopic.php?t=4728&start=15

	*flags = 0;
	if (((*r & 0x0F) + (other & 0x0F)) & 0x10) *flags |= HC_MASK;
	if (((*r & 0xFF) + other) & 0x100) *flags |= CF_MASK;

	uint16_t nn;
	if (other & 128) {
		nn = (~other + 1) & 0xFF;	//For some reason if you don't and the result with 0xFF,
									//the high byte will be 0xFF?
		*r -= nn;
	}
		
	else {
		nn = other;
		*r += nn;
	}
		
}