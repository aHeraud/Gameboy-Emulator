#include<stdio.h>
#include<stdlib.h>	//For malloc
#include<assert.h>
#include<string.h>
#include"mmu.h"

#include"timer.h"
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

//TODO: make low 3 bits of stat read only

//helpers
size_t load_file(const char* path, uint8_t *buffer, int length) {
	FILE* file = fopen(path, "rb");
	if (file == NULL) {
		//failed to open file for reading
		return 0;
	}

	size_t size = fread(buffer, sizeof(uint8_t), length, file);
	fclose(file);
	return size;
}

size_t get_cart_ram_size() {
	uint8_t ram_size = read_byte(0x0149);	//read ram size from cartridge header
	switch(ram_size) {
		case 0:
			if(mmu_info.mbc_type == MBC2)
				return 512;	//mbc2 has 512x4-bits of ram
			else
				return 0;
		case 1:
			return 0x800;    //2KB
		case 2:
			return 0x2000;  //8KB
		case 3:
			return 0x8000;  //32KB
		case 4:
			return 0x20000; //128KB
		case 5:
			return 0x1000;  //64KB
		default:
			return 0;	//invalid entry in header
	}
}

bool cart_has_battery() {
	uint8_t capabilities = memory.cart_rom[0x147];
	switch(capabilities) {
		case 3:
		case 6:
		case 8:
		case 0x0D:
		case 0x0F:
		case 0x10:
		case 0x13:
		case 0x17:
		case 0x1B:
		case 0x1E:
		case 0x22:
		case 0xFF:
			return true;
		default:
			return false;
	}
}



int mmu_init() {
	//Lazily allocate the max amount of space for the rom (256 16KB banks), and ram (16 banks of 8K). (MBC1 values)
	//TODO: read cartridge header to decide how much space to allocate to store rom (same for ram)
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
	size_t size = load_file(rom_file, memory.cart_rom, (256 * 0x4000));\
	if(size == 0 || size < 0x8000)	//TODO: check somewhere else to make sure it's a valid rom
		return -1;	//failed to read from file

	mmu_info.rom_filename = rom_file;

	//TODO: fix mbc detection
	mmu_info.mbc_type = get_mbc_type(memory.cart_rom[0x0147]);

	//TODO: better way to init mbc
	mbc1_init();
	mbc2_init();
	mbc3_init();

	return 0;
}


int load_ram(const char* ram_file) {
	if(!cart_has_battery())
		return 0;	//no battery backed ram

	mmu_info.ram_filename = ram_file;
	size_t size = load_file(ram_file, memory.cart_ram, (16 * 0x2000));
	if(size == 0)
		return -1;	//TODO: handle error?
	return 0;
}

int save_ram(const char* filename) {
	if(!cart_has_battery())
		return 0;	//no battery backed ram

	//create a temporary file
	char tempfn[strlen(filename) + 4];
	strcpy(tempfn, filename);
	strcat(tempfn, ".bak");

	FILE *ram = fopen(tempfn, "wb");
	if (ram == NULL) {
		return -1;
	}

	size_t size = fwrite(memory.cart_ram, sizeof(uint8_t), get_cart_ram_size(), ram);

	fclose(ram);

	if (size != get_cart_ram_size())
		return -1;

	//if no error delete old file(if it exists) and rename new file
	remove(filename);
	rename(tempfn, filename);

	return 0;
}


void dma(uint8_t start_addr) {
	//TODO: set a flag and only allow cpu to access hram until the is "over"
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
	write_byte(0xFF4D, 0x7E);	//maybe?
	write_byte(0xFFFF, 0x00);	//IE
}

inline uint8_t read_byte(uint16_t address) {
	if (address < 0x8000) {
		switch (mmu_info.mbc_type) {
			case NO_MBC:
				return memory.cart_rom[address];
			case MBC1:
				return mbc1_read_byte_rom(address);
			case MBC2:
				return mbc2_read_byte_rom(address);
			case MBC3:
				return mbc3_read_byte_rom(address);
				//case MBC5:
				//	return read_byte_mbc5(address);
			default:
				assert(false);
				return 0;
		}
	}
	else if (address < 0xA000) {
		//can't access vram in stat mode 3
		if((memory.io[0x41] & 3) < 3)
			return memory.vram[address - 0x8000];
        else
            return 0;
	}
	else if (address < 0xC000) {
		switch (mmu_info.mbc_type) {
			case NO_MBC:
				return memory.cart_ram[address - 0xA000];
			case MBC1:
				return mbc1_read_byte_ram(address);
			case MBC2:
				return mbc2_read_byte_ram(address);
			case MBC3:
				return mbc3_read_byte_ram(address);
				//case MBC5:
				//	return mbc5_read_byte_ram(address);
			default:
				assert(false);
				return 0;
		}
	}
	else if (address < 0xE000)
		return memory.wram[address - 0xC000];
	else if (address < 0xFE00)
		return memory.wram[address - 0xE000];	//Mirror of wram (ECHO)
	else if (address < 0xFEA0) {
        if((memory.io[0x41] & 3) < 2)\
            return memory.oam[address - 0xFE00];
        else
            return 0;
    }
	else if (address < 0xFF00)
		return 0;
	else if (address < 0xFF80) {
		if(address == 0xFF04)
			return (timer_div >> 8) & 0xFF;
		else
			return memory.io[address - 0xFF00];
	}
	else if (address < 0xFFFF)
		return memory.hram[address - 0xFF80];
	else
		return memory.interrupt_enable;
}

inline void write_byte(uint16_t address, uint8_t value) {
	if (address < 0x8000) {
		switch (mmu_info.mbc_type) {
			case NO_MBC:
				break;
			case MBC1:
				mbc1_write_byte_rom(address, value);
				break;
			case MBC2:
				mbc2_write_byte_rom(address, value);
				break;
			case MBC3:
				mbc3_write_byte_rom(address,value);
				break;
				//case MBC5:
				//	return read_byte_mbc5(address);
			default:
				assert(false);
		}
	}
	else if (address < 0xA000) {
		//can't access vram in mode 3
		if((memory.io[0x41] & 3) < 3)
			memory.vram[address - 0x8000] = value;
	}
	else if (address < 0xC000) {
		switch (mmu_info.mbc_type) {
			case NO_MBC:
				memory.cart_ram[address - 0xA000] = value;
				break;
			case MBC1:
				mbc1_write_byte_ram(address, value);
				break;
			case MBC2:
				mbc2_write_byte_ram(address, value);
				break;
			case MBC3:
				mbc3_write_byte_ram(address, value);
				break;
				//case MBC5:
				//	mbc5_write_byte_ram(address);
			default:
				assert(false);
		}
	}
	else if (address < 0xE000)
		memory.wram[address - 0xC000] = value;
	else if (address < 0xFE00)
		memory.wram[address - 0xE000] = value;	//Mirror of wram (ECHO)
	else if (address < 0xFEA0) {
        //cpu can't access oam during mode 2 or 3
        if((memory.io[0x41] & 3) < 2)
            memory.oam[address - 0xFE00] = value;
    }
	else if (address < 0xFF00)
		return;	//unusable memory
	else if (address < 0xFF80) {	//IO
        if(address == 0xFF41) { //low 3 bits of STAT are read only
            memory.io[0x41] &= 0xFC;
            memory.io[0x41] |= (value & 0xFC);
        }
		if (address == 0xFF46)
			dma(value);	//Start LCD OAM DMA
		else if (address == 0xFF04)	//DIV: If written to, reset
			timer_div = 0;
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

