#ifndef MBC5_H
#define MBC5_H

#include<stdint.h>

uint8_t read_byte_mbc5(uint16_t address);
void write_byte_mbc5(uint16_t address, uint8_t value);

#endif