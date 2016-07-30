#ifndef MBC_H
#define MBC_H

#include<stdint.h>
#include<stdbool.h>

typedef struct {
	bool ram_select;
	bool ram_enable;
	uint8_t bank_low;	//Low 5 bits
	uint8_t bank_high;	//High 2 bits of rom bank / ram bank number
} mbc1_t;

extern mbc1_t mbc1;

void mbc1_init();

uint8_t mbc1_read_byte_rom(uint16_t address);
uint8_t mbc1_read_byte_ram(uint16_t address);

void mbc1_write_byte_rom(uint16_t address, uint8_t value);
void mbc1_write_byte_ram(uint16_t address, uint8_t value);

//uint8_t read_byte_mbc1(uint16_t address);   //DEPRECATED
//void write_byte_mbc1(uint16_t address, uint8_t value);  //DEPRECATED



#endif