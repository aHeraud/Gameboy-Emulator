#include<stdint.h>
#include"mbc1.h"
#include"mmu.h"

mbc1_t mbc1;

void mbc1_init() {
	mbc1.ram_enable = true;
	mbc1.ram_select = false;
	mbc1.bank_low = 1;
	mbc1.bank_high = 0;
}

uint8_t read_byte_mbc1(uint16_t address) {
	if (address < 0x4000)	//Rom bank 0
		return memory.cart_rom[address];
	else if (address < 0x8000) {
		//Switchable rom bank
		uint8_t selected_rom_bank = mbc1.bank_low;
		if (mbc1.ram_select == false) selected_rom_bank |= (mbc1.bank_high << 5);
		return memory.cart_rom[address - 0x4000 + (ROM_BANK_SIZE * selected_rom_bank)];
	}
	else if (address < 0xA000)
		return memory.vram[address - 0x8000];
	else if (address < 0xC000) {
		//Switchable ram bank
		if (mbc1.ram_enable == false)
			return 0;
		uint8_t selected_ram_bank = 0;
		if (mbc1.ram_select == true) selected_ram_bank = mbc1.bank_high;
		return memory.cart_ram[address - 0xA000 + (RAM_BANK_SIZE * selected_ram_bank)];
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

void write_byte_mbc1(uint16_t address, uint8_t value) {
	if (address < 0x2000)
		mbc1.ram_enable = ((value & 0x0A) == 0x0A) ? true : false;
	else if (address < 0x4000)
		mbc1.bank_low = (value == 0) ? 1 : (value & 0x1F);
	else if (address < 0x6000)
		mbc1.bank_high = value & 3;
	else if (address < 0x8000)
		mbc1.ram_select = (value & 1) ? true : false;
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