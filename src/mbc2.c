#include<assert.h>
#include<stdbool.h>

#include"mbc2.h"

mbc2_t mbc2;

void mbc2_init() {
	mbc2.ram_enable = true;
	mbc2.rom_bank = 1;
}

uint8_t mbc2_read_byte_rom(uint16_t address) {
	if(address < 0x4000) {
		//Rom bank 0
		return memory.cart_rom[address];
	}
	else if(address < 0x8000) {
		//Rom banks 1 - 0x0F
		return memory.cart_rom[address - 0x4000 + (ROM_BANK_SIZE & (mbc2.rom_bank & 0x0F))];
	}
	else {
		assert(false);
		return 0;
	}
}

uint8_t mbc2_read_byte_ram(uint16_t address) {
	if(address >= 0xA000 && address < 0xC000) {
		if(mbc2.ram_enable) {
			uint16_t offset = address - 0xA000;
			if(offset < 512)
				return memory.cart_ram[offset] & 0x0F;	//4-bits
			else {
				return 0;	//address is out of bounds (maybe check what a real mbc2 does?)
			}
		}
		else {	//ram is disabled
			return 0;
		}
	}
	else {
		assert(false);	//invalid address
		return 0;
	}
}

void mbc2_write_byte_rom(uint16_t address, uint8_t value) {
	//in the mbc2, the lsb of the upper address byte must be 0 for enabling/disabling ram,
	// and it must be 1 to select rom banks: http://gbdev.gg8.se/wiki/articles/MBC2
	if(address < 0x2000 && (address & 0x0100) == 0) {
		//enable/disable ram
		if((value & 0x0A) == 0x0A)
			mbc2.ram_enable = true;
		else
			mbc2.ram_enable = false;
	}
	else if(address < 0x4000 && (address & 0x0100) == 0x0100) {
		//select rom bank number
		mbc2.rom_bank = value & 0x0F;
	}
}

void mbc2_write_byte_ram(uint16_t address, uint8_t value) {
	//the mbc2 only has 512x4-bit ram
	if(address >= 0xA000 && address < 0xC000) {
		uint16_t offset = address - 0xA000;
		if(offset < 512)
			return;	//only 512 'bytes' of ram
		else
			memory.cart_ram[offset] = value & 0x0F;	//4-bits (im assuming it's the lower nibble)
	}
	else {
		assert(false);	//invalid address (cartridge ram is from 0xA000 -> 0xC000)
	}
}