#include<stdint.h>
#include<assert.h>
#include<stdbool.h>

#include"mbc3.h"
#include"mmu.h"

mbc3_t mbc3;

void mbc3_update_rtc() {
	//TODO: mbc3 rtc
	//note: mbc3 data needs to be saved, because of the day counter carry bit (or just pretend it doesn't exist!)
}

void mbc3_init() {
	//TODO
	mbc3.ram_and_timer_enable = true;
	mbc3.clock_latch = false;
	mbc3.clock_latch_00 = false;
	mbc3.rom_bank = 1;
	mbc3.ram_bank = 0;
}

uint8_t mbc3_read_byte_rom(uint16_t address) {
	if (address < 0x4000)	//Rom bank 0
		return memory.cart_rom[address];
	else if (address < 0x8000) {
		//Switchable rom bank
		return memory.cart_rom[address - 0x4000 + (ROM_BANK_SIZE * mbc3.rom_bank)];
	}
	else {
		assert(false);	//invalid address
        return 0;
	}
}

uint8_t mbc3_read_byte_ram(uint16_t address) {
	if (address >= 0xA000 && address < 0xC000) {
		//Switchable ram bank
		if (mbc3.ram_and_timer_enable == false)
			return 0;
		//If ram_bank == 8,9,A,B,C then read from rtc, not ram
		if (mbc3.ram_bank < 4) {
			//Read from ram
			return memory.cart_ram[address - 0xA000 + (RAM_BANK_SIZE * mbc3.ram_bank)];
		}
		else {
			//Read from rtc
			//TODO: clock latching
			mbc3_update_rtc();
			switch (mbc3.ram_bank) {
				case 0x08:
					return mbc3.rtc.s;
					break;
				case 0x09:
					return mbc3.rtc.m;
					break;
				case 0x0A:
					return mbc3.rtc.h;
					break;
				case 0x0B:
					return mbc3.rtc.dl;
					break;
				case 0x0C:
					return mbc3.rtc.dh;
					break;
				default:
					return 0;
			}
		}
	}
	else {
		assert(false);	//invalid address (0xA000 -> 0xBFFF)
        return 0;
	}
}

void mbc3_write_byte_rom(uint16_t address, uint8_t value) {
	if (address < 0x2000)
		mbc3.ram_and_timer_enable = ((value & 0x0A) == 0x0A) ? true : false;
	else if (address < 0x4000)
		mbc3.rom_bank = (value) ? (value & 0x7F) : 1;	//cant set it to bank 0
	else if (address < 0x6000)
		mbc3.ram_bank = value & 0x0F;
	else if (address < 0x8000) {
		//Latch clock data
		if (value == 0)
			mbc3.clock_latch_00 = true;
		else if (value == 1 && mbc3.clock_latch_00) {
			mbc3.clock_latch = !mbc3.clock_latch;
			mbc3.clock_latch_00 = false;
		}
	}
	else {
		assert(false);	//Invalid address
	}
}

void mbc3_write_byte_ram(uint16_t address, uint8_t value) {
	if(address >= 0xA000 && address < 0xC000) {
		if (mbc3.ram_and_timer_enable == false)
			return;
		//If ram_bank == 8,9,A,B,C then read from rtc, not ram
		if (mbc3.ram_bank < 4) {
			//Read from ram
			uint16_t offset = address - 0xA000;
			memory.cart_ram[offset + (RAM_BANK_SIZE * mbc3.ram_bank)] = value;
		}
		else {
			//Read from rtc
			//TODO: clock latching
			mbc3_update_rtc();
			switch (mbc3.ram_bank) {
				case 0x08:
					mbc3.rtc.s = value;
					break;
				case 0x09:
					mbc3.rtc.m = value;
					break;
				case 0x0A:
					mbc3.rtc.h = value;
					break;
				case 0x0B:
					mbc3.rtc.dl = value;
					break;
				case 0x0C:
					mbc3.rtc.dh = value;
					break;
			}
		}
	}
	else {
		assert(false);	//Invalid address (0xA000 -> 0xBFFF)
	}
}