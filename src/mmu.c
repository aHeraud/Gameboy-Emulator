#include<stdio.h>
#include<stdlib.h>	//For malloc
#include<assert.h>
#include"mmu.h"

#include"mbc1.h"
#include"mbc2.h"
#include"mbc3.h"
#include"mbc5.h"

memory_t memory;
mmu_info_t mmu_info;

/*-----------------------------------------------------------
ROM 0	: 0000 - 3FFF  =  0000000000000000 - ‭0011111111111111‬
ROM 1	: 4000 - 7FFF  =  ‭0100000000000000‬ - ‭0111111111111111‬	
VRAM	: 8000 - 9FFF  =  ‭1000000000000000‬ - ‭1001111111111111‬
RAM		: A000 - BFFF  =  ‭1010000000000000‬ - ‭1011111111111111‬
WRAM 0	: C000 - CFFF  =  ‭1100000000000000‬ - ‭1100111111111111‬
WRAM 1	: D000 - DFFF  =  ‭1101000000000000‬ - ‭1101111111111111‬
ECHO	: E000 - FDFF  =  ‭1110000000000000‬ - ‭1111110111111111‬
OAM		: FE00 - FE9F  =  ‭1111111000000000‬ - ‭1111111010011111‬
Unusable: FEA0 - FEFF  =  ‭1111111010100000‬ - ‭1111111011111111‬
IO		: FF00 - FF7F  =  ‭1111111100000000‬ - ‭1111111101111111‬
HRAM	: FF80 - FFFE  =  ‭1111111110000000‬ - ‭1111111111111110‬
IER		: FFFF - FFFF  =  1111111111111111 - ‭1111111111111111
-----------------------------------------------------------*/

int mmu_init() {
	//Lazily allocate the max ammount of space for the rom (256 16KB banks), and ram (16 banks of 8K). (MBC1 values)
	//TODO: read cartridge header to decide how much space to allocate to store rom
	memory.cart_rom = (uint8_t *)malloc(256 * 0x4000 * sizeof(uint8_t));
	if (!memory.cart_rom)
		return -1;

	memory.cart_ram = (uint8_t *)malloc(16 * 0x2000 * sizeof(uint8_t));
	if (!memory.cart_ram)
		return -1;

	mmu_reset();

	return 0;
}

void mmu_destroy() {
	if (memory.cart_rom) {
		free(memory.cart_rom);
		memory.cart_rom = NULL;
	}
	if (memory.cart_ram) {
		free(memory.cart_ram);
		memory.cart_ram = NULL;
	}
}

int load_rom(const char* rom_file) {
	FILE* rom = fopen(rom_file, "r");
	if (rom == NULL) {
		//failed to open file for reading
		return -1;
	}
	mmu_info.rom_file_size = fread(memory.cart_rom, sizeof(uint8_t), (256 * 0x4000), rom);
	mmu_info.rom_filename = rom_file;

	//TODO: fix mbc detection
	mmu_info.mbc_type = get_mbc_type(memory.cart_rom[0x0147]);

	//TODO:init mbc
	mbc1_init();

	return 0;
}

int load_ram(const char* ram_file) {
	FILE* ram = fopen(ram_file, "r");
	if (ram == NULL) {
		//failed to open file for reading
		return -1;
	}

	mmu_info.ram_file_size = fread(memory.cart_ram, sizeof(uint8_t), (16 * 0x2000), ram);
	mmu_info.ram_filename = ram_file;

	return 0;
}

int save_ram() {
	FILE *ram = fopen(mmu_info.ram_filename, "w");
	if (ram == NULL) {
		return -1;
	}
	size_t size = fwrite(memory.cart_ram, sizeof(uint8_t), mmu_info.ram_file_size, ram);
	if (size != mmu_info.ram_file_size) 
		return -1;
	return 0;
}

void dma(uint8_t start_addr) {
	//TODO: don't do this all in the same tick?
	if (start_addr > 0xF1)
		return;
	else {
		uint16_t address = start_addr << 8;
		for (int i = 0; i <= 0x9F; ++i) {
			write_byte(0xFE00 + i, read_byte(address + i));
		}
	}
}

void mmu_reset() {
	write_byte(0xFF00, 0xFF);	//JOYP
	write_byte(0xFF01, 0);	//SB
	write_byte(0xFF02, 0);	//SC
	write_byte(0xFF04, 0);	//DIV
	write_byte(0xFF05, 0);	//TIMA
	write_byte(0xFF06, 0);	//TMA
	write_byte(0xFF07, 0xF8);	//TAC
	write_byte(0xFF10, 0x80);
	write_byte(0xFF11, 0xBF);
	write_byte(0xFF12, 0xF3);
	write_byte(0xFF14, 0xBF);
	write_byte(0xFF16, 0x3F);
	write_byte(0xFF17, 0);
	write_byte(0xFF19, 0xBF);
	write_byte(0xFF1A, 0x7F);
	write_byte(0xFF1B, 0xFF);
	write_byte(0xFF1C, 0x9F);
	write_byte(0xFF1E, 0xBF);
	write_byte(0xFF20, 0xFF);
	write_byte(0xFF21, 0x00);
	write_byte(0xFF22, 0x00);
	write_byte(0xFF23, 0xBF);
	write_byte(0xFF24, 0x77);
	write_byte(0xFF25, 0xF3);
	write_byte(0xFF26, 0xF1);
	write_byte(0xFF40, 0x91);	//LCDC
	write_byte(0xFF41, 0x85);	//STAT
	write_byte(0xFF42, 0x00);	//SCY
	write_byte(0xFF43, 0x00);	//SCX
	write_byte(0xFF45, 0x00);	//LYC
	write_byte(0xFF47, 0xFC);	//BGP
	write_byte(0xFF48, 0xFF);	//OBP0
	write_byte(0xFF49, 0xFF);	//OBP1
	write_byte(0xFF4A, 0x00);	//WY
	write_byte(0xFF4B, 0x00);	//WX
	write_byte(0xFF4D, 0x7E);	//maybe? (TODO:check)
	write_byte(0xFFFF, 0x00);	//IE
}

uint8_t read_byte(uint16_t address) {
	switch (mmu_info.mbc_type) {
	case NO_MBC:
		return read_byte_no_mbc(address);
	case MBC1:
		return read_byte_mbc1(address);
	//case MBC2:
	//	return read_byte_mbc2(address);
	case MBC3:
		return read_byte_mbc3(address);
	//case MBC5:
	//	return read_byte_mbc5(address);
	default:
		assert(false);
	}
	return 0;
}

void write_byte(uint16_t address, uint8_t value) {
	switch (mmu_info.mbc_type) {
	case NO_MBC:
		write_byte_no_mbc(address, value);
		break;
	case MBC1:
		write_byte_mbc1(address, value);
		break;
	//case MBC2:
		//write_byte_mbc2(address, value);
	//	break;
	case MBC3:
		write_byte_mbc3(address, value);
		break;
	//case MBC5:
	//	write_byte_mbc5(address, value);
	//	break;
	default:
		assert(false);
	}
}

inline uint8_t read_byte_no_mbc(uint16_t address) {
	if (address < 0x8000)
		return memory.cart_rom[address];
	else if (address < 0xA000)
		return memory.vram[address - 0x8000];
	else if (address < 0xC000)
		return memory.cart_ram[address - 0xA000];
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

inline void write_byte_no_mbc(uint16_t address, uint8_t value) {
	if (address < 0x8000)
		return;	//can't write to rom (except for mbcs)
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

mbc_type_enum get_mbc_type(uint8_t cart_type) {
	if (cart_type == 0 || cart_type == 8 || cart_type == 9)
		return NO_MBC;
	else if (cart_type > 0 && cart_type < 4)
		return MBC1;
	else if (cart_type > 4 && cart_type < 7)
		return MBC2;
	else if (cart_type > 0x0E && cart_type < 0x14)
		return MBC3;
	else if (cart_type > 0x14 && cart_type < 0x18)
		return MBC4;
	else if (cart_type > 0x18 && cart_type < 0x20)
		return MBC5;

	return NO_MBC;	//invalid header
}