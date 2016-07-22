#include"register8.h"
#include<stdint.h>

#include<stdio.h>

const uint8_t ZF_MASK = 128;
const uint8_t SF_MASK = 64;
const uint8_t HC_MASK = 32;
const uint8_t CF_MASK = 16;

//zf = ~((((result) * 0x7F) + 0x7F) | (result)) & ZF_MASK;

void add8(uint8_t *r8, uint8_t other, uint8_t *flags)
{
	//Flags: Z 0 H C
	uint8_t result = *r8 + other;
	*flags = 0;
	if (result == 0) *flags |= ZF_MASK;
	if (((*r8 & 0x0F) + (other & 0x0F)) > 0x0F) *flags |= HC_MASK;
	if ((result < *r8) || (result < other)) *flags |= CF_MASK;
	*r8 = result;
}

void adc(uint8_t *r8, uint8_t other, uint8_t *flags)
{
	//Flags: Z 0 H C
	uint8_t cy = (*flags & CF_MASK) >> 4;
	uint16_t result = *r8 + other + cy;
	*flags = 0;
	if ((result & 0xFF) == 0) *flags |= ZF_MASK;
	if ((*r8 & 0x0F) + (other & 0x0F) + cy > 0x0F) *flags |= HC_MASK;
	if (result & 0xFF00) *flags |= CF_MASK;
	*r8 = (uint8_t)(result & 0xFF);
}

void sub(uint8_t *r8, uint8_t other, uint8_t *flags)
{
	//Flags: Z 1 H C
	uint8_t result = *r8 - other;
	*flags = SF_MASK;
	if (other > *r8) *flags |= CF_MASK;
	if ((other & 0x0F) > (*r8 & 0x0F)) *flags |= HC_MASK;
	if (result == 0) *flags |= ZF_MASK;
	*r8 = result;
}

void sbc(uint8_t *r8, uint8_t other, uint8_t *flags)
{
	//Flags: Z 1 H C
	uint8_t cy = (*flags & CF_MASK) >> 4;
	int16_t op = (int16_t)(other + cy);
	int16_t result = (int16_t)*r8 - op;
	
	*flags = SF_MASK;
	if ((result & 0xFF) == 0) *flags |= ZF_MASK;
	if (result & 0xFF00) *flags |= CF_MASK;
	if ((op & 0x0F) > (*r8 & 0x0F) || (other & 0x0F) + cy > 0x0F) *flags |= HC_MASK;
	
	*r8 = (uint8_t)result;
}

void and(uint8_t *r8, uint8_t other, uint8_t *flags)
{
	//Flags: Z 0 1 0
	*r8 &= other;
	*flags = HC_MASK;
	if (*r8 == 0) *flags |= ZF_MASK;
}

void xor(uint8_t *r8, uint8_t other, uint8_t *flags)
{
	//Flags: Z 0 0 0
	*r8 ^= other;
	*flags = 0;
	if (*r8 == 0) *flags |= ZF_MASK;
}

void or (uint8_t *r8, uint8_t other, uint8_t *flags)
{
	//Flags: Z 0 0 0
	*r8 |= other;
	*flags = 0;
	if (*r8 == 0) *flags |= ZF_MASK;
}

void cp(uint8_t r8, uint8_t other, uint8_t *flags)
{
	//Flags: Z 1 H C
	uint8_t op = r8;
	sub(&op, other, flags);
}

void inc(uint8_t *r8, uint8_t *flags)
{
	//Flags: Z 0 H -
	uint8_t	result = *r8 + 1;
	*flags &= CF_MASK;
	if (result == 0) *flags |= ZF_MASK;
	if (((*r8 & 0x0F) + 1) > 0x0F) *flags |= HC_MASK;
	*r8 = result;
}

void dec(uint8_t *r8, uint8_t *flags)
{
	//Flags: Z 1 H -
	uint8_t fake_flags = 0;
	sub(r8, 1, &fake_flags);
	fake_flags &= 0b11100000;
	*flags &= CF_MASK;
	*flags |= fake_flags;
}

void rlc(uint8_t *r8, uint8_t *flags)
{
	//Flags: Z 0 0 C
	uint8_t msb = *r8 & 128;
	*r8 = (*r8 << 1) | (msb >> 7);
	*flags = 0;
	if (*r8 == 0) *flags |= ZF_MASK;
	if (msb) *flags |= CF_MASK;
}

void rrc(uint8_t *r8, uint8_t *flags)
{
	//Flags: Z 0 0 C
	uint8_t lsb = *r8 & 1;
	*r8 = (*r8 >> 1) | (lsb << 7);
	*flags = 0;
	if (*r8 == 0) *flags |= ZF_MASK;
	if (lsb) *flags |= CF_MASK;
}

void rl(uint8_t *r8, uint8_t *flags)
{
	//Flags: Z 0 0 C
	//9-bit shift through carry
	uint8_t msb = *r8 & 128;
	*r8 = *r8 << 1;
	if (*flags & CF_MASK) *r8 |= 1;
	*flags = 0;
	if (*r8 == 0) *flags |= ZF_MASK;
	if (msb) *flags |= CF_MASK;
}

void rr(uint8_t *r8, uint8_t *flags)
{
	//Flags: Z 0 0 C
	//9-bit rotate through carry
	uint8_t lsb = *r8 & 1;
	*r8 = (*r8 >> 1);
	if (*flags & CF_MASK) *r8 |= 128;
	*flags = 0;
	if (*r8 == 0) *flags |= ZF_MASK;
	if (lsb) *flags |= CF_MASK;
}

void sla(uint8_t *r8, uint8_t *flags)
{
	//Flags: Z 0 0 C
	uint8_t msb = *r8 & 128;
	*r8 = *r8 << 1;
	*flags = 0;
	if (*r8 == 0) *flags |= ZF_MASK;
	if (msb) *flags |= CF_MASK;
}

void sra(uint8_t *r8, uint8_t *flags)
{
	//Flags: Z 0 0 C
	uint8_t msb = *r8 & 128;
	uint8_t lsb = *r8 & 1;
	*r8 = (*r8 >> 1) | msb;
	*flags = 0;
	if (*r8 == 0) *flags |= ZF_MASK;
	if (lsb) *flags |= CF_MASK;
}

void srl(uint8_t *r8, uint8_t *flags) {
	//Flags: Z 0 0 C
	uint8_t lsb = *r8 & 1;
	*r8 = *r8 >> 1;
	*flags = 0;
	if (*r8 == 0) *flags |= ZF_MASK;
	if (lsb) *flags |= CF_MASK;
}

void swap(uint8_t *r8, uint8_t *flags) {
	//Flags: Z 0 0 0
	uint8_t low = (*r8 & 0b1111);
	uint8_t high = (*r8 >> 4) & 0b00001111;
	*r8 = (low << 4) | high;

	*flags = 0;
	if (*r8 == 0) *flags = ZF_MASK;
}

void bit(uint8_t r8, uint8_t *flags, uint8_t n)
{
	//Flags: Z 0 1 -
	uint8_t cy = *flags & CF_MASK;
	r8 &= (1 << n);
	*flags = 0;
	*flags = HC_MASK;
	*flags |= cy;
	if (r8 == 0) *flags |= ZF_MASK;
}

void set(uint8_t *r8, uint8_t n)
{
	*r8 |= (1 << n);
}

void res(uint8_t *r8, uint8_t n)
{
	*r8 &= ~(1 << n);
}

