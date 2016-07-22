#ifndef MBC2_H
#define MBC2_H

#include<stdint.h>
#include<stdbool.h>

typedef struct {
	bool ram_enable;
	uint8_t rom_bank;
} mbc2_t;

uint8_t read_byte_mbc2(uint16_t address);
void write_byte_mbc2(uint16_t address, uint8_t value);

#endif