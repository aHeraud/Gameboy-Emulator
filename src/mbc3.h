#ifndef MBC3_H
#define MBC3_H

#include<stdint.h>
#include<stdbool.h>

typedef struct {
	uint8_t s;
	uint8_t m;
	uint8_t h;
	uint8_t dl;
	uint8_t dh;
} rtc_t;

typedef struct {
	bool ram_and_timer_enable;
	bool clock_latch, clock_latch_00;
	uint8_t rom_bank;	//7bits
	uint8_t ram_bank;	//4 bits? also used to select rtc registers
	rtc_t rtc;
} mbc3_t;

extern mbc3_t mbc3;

void mbc3_init();

uint8_t mbc3_read_byte_rom(uint16_t address);
uint8_t mbc3_read_byte_ram(uint16_t address);

void mbc3_write_byte_rom(uint16_t address, uint8_t value);
void mbc3_write_byte_ram(uint16_t address, uint8_t value);

/*
uint8_t read_byte_mbc3(uint16_t address);
void write_byte_mbc3(uint16_t address, uint8_t value);
*/

#endif