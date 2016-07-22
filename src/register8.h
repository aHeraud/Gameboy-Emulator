#ifndef REGISTER8_H
#define REGISTER8_H

/* Z80 8-bit registers */
#include<stdint.h>


extern const uint8_t ZF_MASK;
extern const uint8_t SF_MASK;
extern const uint8_t HC_MASK;
extern const uint8_t CF_MASK;


/** Arithmetic/logical instructions **/

/**
The value of other is added to r8
Flags: Z 0 H C
*/
void add8(uint8_t *r8, uint8_t other, uint8_t *flags);

/**
The value of other + the carry flag is added to r8
Flags: Z 0 H C
*/
void adc(uint8_t *r8, uint8_t other, uint8_t *flags);

/**
r8 = r8 - other
Flags:
*/
void sub(uint8_t *r8, uint8_t other, uint8_t *flags);

void sbc(uint8_t *r8, uint8_t other, uint8_t *flags);

/**
r8 = r8 * other
Flags: Z 0 1 0
*/
void and(uint8_t *r8, uint8_t other, uint8_t *flags);

/**
r8 = r8 ^ other
Flags: Z 0 0 0
*/
void xor(uint8_t *r8, uint8_t other, uint8_t *flags);

/**
r8 = r8 | other
*/
void or (uint8_t *r8, uint8_t other, uint8_t *flags);

void cp(uint8_t r8, uint8_t other, uint8_t *flags);

/**
r8++
Flags: Z 0 H -
*/
void inc(uint8_t *r8, uint8_t *flags);

/**
r8--
Flags: Z 1 H -
*/
void dec(uint8_t *r8, uint8_t *flags);



/** Shifts, Rotates, and single bit instructions **/

/**
Rotate Left
Flags: Z00C
The bit leaving on the left is coppied to the CF and bit 0
*/
void rlc(uint8_t *r8, uint8_t *flags);

/**
Rotate Right
Flags: Z 0 0 C
The bit leaving on the right is copied to the CF and bit 7
*/
void rrc(uint8_t *r8, uint8_t *flags);

/**
9-bit Rotate Left through carry flag.
The value of the carry flag is copied into bit 0, and the value leaving
bit 7 is copied into the carry flag.
Flags: Z 0 0 C
*/
void rl(uint8_t *r8, uint8_t *flags);

/**
9-bit rotate left through carry.
The value of the carry is copied into bit 7, and the value of bit 0 is copied
into the carry flag.
Flags: Z 0 0 C
*/
void rr(uint8_t *r8, uint8_t *flags);

/**
Signed left shift
Flags: Z 0 0 C
*/
void sla(uint8_t *r8, uint8_t *flags);

/**
Signed right shift
Flags: Z 0 0 C
Note: conflicting information on whether sra sets carry flag
*/
void sra(uint8_t *r8, uint8_t *flags);

/**
Swap low and high nibbles
Flags: Z 0 0 0
*/
void swap(uint8_t *r8, uint8_t *flags);

/**
Shift Right Logical
Flags: Z 0 0 C
*/
void srl(uint8_t *r8, uint8_t *flags);

/**
Test bit n
Flags: Z 0 1 -
*/
void bit(uint8_t r8, uint8_t *flags, uint8_t n);

/**
Set bit n to 1
Flags: - - - -
*/
void set(uint8_t *r8, uint8_t n);

/**
Set bit n to 0
Flags: - - - -
*/
void res(uint8_t *r8, uint8_t n);

#endif