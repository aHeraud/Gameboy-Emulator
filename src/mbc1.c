#include<stdint.h>
#include<assert.h>
#include<stdbool.h>
#include"mbc1.h"
#include"mmu.h"

mbc1_t mbc1;

void mbc1_init() {
	mbc1.ram_enable = true;
	mbc1.ram_select = false;
	mbc1.bank_low = 1;
	mbc1.bank_high = 0;
}

uint8_t mbc1_read_byte_rom(uint16_t address) {
	if (address < 0x4000)	//Rom bank 0
		return memory.cart_rom[address];
	else if (address < 0x8000) {
		//Switchable rom bank
		uint8_t selected_rom_bank = mbc1.bank_low;
		if (mbc1.ram_select == false) selected_rom_bank |= (mbc1.bank_high << 5);
		return memory.cart_rom[address - 0x4000 + (ROM_BANK_SIZE * selected_rom_bank)];
	}
	else {
		assert(false);	//address has to be mapped to the cartridge rom, so from 0 -> 0x8000
		return 0;
	}
}

uint8_t mbc1_read_byte_ram(uint16_t address) {
	if (address >= 0xA000 && address < 0xC000) {
		//Switchable ram bank
		if (mbc1.ram_enable == false)
			return 0;
		uint8_t selected_ram_bank = 0;
		if (mbc1.ram_select == true) selected_ram_bank = mbc1.bank_high;
		return memory.cart_ram[address - 0xA000 + (RAM_BANK_SIZE * selected_ram_bank)];
	}
	else {
		assert(false); //address has to be mapped to the cartridge ram, so from 0xA000 -> 0xC000
		return 0;
	}
}

void mbc1_write_byte_rom(uint16_t address, uint8_t value) {
	if (address < 0x2000)
		mbc1.ram_enable = ((value & 0x0A) == 0x0A) ? true : false;
	else if (address < 0x4000)
		mbc1.bank_low = (value == 0) ? 1 : (value & 0x1F);
	else if (address < 0x6000)
		mbc1.bank_high = value & 3;
	else if (address < 0x8000)
		mbc1.ram_select = (value & 1) ? true : false;
	else {
		//address out of range
		assert(false);	//rom is from address 0 -> 0x8000
	}
}

void mbc1_write_byte_ram(uint16_t address, uint8_t value) {
	if (address >= 0xA000 && address < 0xC000) {
		//Switchable ram bank
		if (mbc1.ram_enable == false) {
			return;
		}

		uint8_t selected_ram_bank = 0;
		if (mbc1.ram_select == true) {
			selected_ram_bank = mbc1.bank_high;
			memory.cart_ram[address - 0xA000 + (RAM_BANK_SIZE * selected_ram_bank)] = value;
		}
	}
	else {
		assert(false); //address has to be mapped to the cartridge ram, so from 0xA000 -> 0xC000
	}
}