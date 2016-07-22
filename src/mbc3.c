#include<stdint.h>

#include"mbc3.h"
#include"mmu.h"

mbc3_t mbc3;

void mbc3_update_rtc() {

}

uint8_t read_byte_mbc3(uint16_t address) {
	if (address < 0x4000)	//Rom bank 0
		return memory.cart_rom[address];
	else if (address < 0x8000) {
		//Switchable rom bank
		return memory.cart_rom[address - 0x4000 + (ROM_BANK_SIZE * mbc3.rom_bank)];
	}
	else if (address < 0xA000)
		return memory.vram[address - 0x8000];
	else if (address < 0xC000) {
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
	else if (address < 0xE000)
		return memory.wram[address - 0xC000];
	else if (address < 0xFE00)
		return memory.wram[address - 0xE000];	//Mirror of wram (ECHO)
	else if (address < 0xFEA0)
		return memory.oam[address - 0xFE00];
	else if (address < 0xFF00)
		return 0;
	else if (address < 0xFF80) {
		return memory.io[address - 0xFF00];
	}
	else if (address < 0xFFFF)
		return memory.hram[address - 0xFF80];
	else
		return memory.interrupt_enable;
}

void write_byte_mbc3(uint16_t address, uint8_t value) {
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
		else if (value == 1) {
			mbc3.clock_latch = !mbc3.clock_latch;
			mbc3.clock_latch_00 = false;
		}
	}
	else if (address < 0xA000)
		memory.vram[address - 0x8000] = value;	//TODO: check video mode
	else if (address < 0xC000)
		memory.cart_ram[address - 0xA000] = value;
	else if (address < 0xE000)
		memory.wram[address - 0xC000] = value;
	else if (address < 0xFE00)
		memory.wram[address - 0xE000] = value;	//Mirror of wram (ECHO)
	else if (address < 0xFEA0)
		memory.oam[address - 0xFE00] = value;
	else if (address < 0xFF00)
		return;	//unusable memory
	else if (address < 0xFF80) {	//IO
		if (address == 0xFF46)
			dma(value);	//Start LCD OAM DMA
		else if (address == 0xFF04)	//DIV: If written to, reset
			memory.io[0x04] = 0;
		else
			memory.io[address - 0xFF00] = value;
	}
	else if (address < 0xFFFF)
		memory.hram[address - 0xFF80] = value;
	else
		memory.interrupt_enable = value;
}