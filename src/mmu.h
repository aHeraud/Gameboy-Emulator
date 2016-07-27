#ifndef MMU_H
#define MMU_H

#include<stdlib.h>  //For size_t
#include<stdint.h>
#include<stdbool.h>

#define ROM_BANK_SIZE 0x4000
#define RAM_BANK_SIZE 0x2000

typedef enum { NO_MBC, MBC1, MBC2, MBC3, MBC4, MBC5 } mbc_type_enum;
mbc_type_enum get_mbc_type(uint8_t cart_type);

typedef struct {
	//uint8_t cart_rom[0x8000];
	uint8_t *cart_rom;
	//uint8_t cart_ram[8192];
	uint8_t *cart_ram;
	uint8_t vram[8192];
	uint8_t wram[8192];
	uint8_t oam[160];
	uint8_t empty[96];	//Not used
	uint8_t io[128];
	uint8_t hram[127];
	uint8_t interrupt_enable;
	
} memory_t;

typedef struct {
	mbc_type_enum mbc_type;
	const char* rom_filename;
	size_t rom_file_size;
	const char* ram_filename;
	size_t ram_file_size;
} mmu_info_t;

extern memory_t memory;
extern mmu_info_t mmu_info;

/* Allocated cartridge memory, returns 0 on success */
int mmu_init();
void mmu_reset();
void mmu_destroy();

//For oam dma
void dma(uint8_t start_address);

uint8_t read_byte(uint16_t address);
void write_byte(uint16_t address, uint8_t value);

uint8_t read_byte_no_mbc(uint16_t address);
void write_byte_no_mbc(uint16_t address, uint8_t value);

/*Loads rom file, returns 0 on success*/
int load_rom(const char* rom_file);
int load_ram(const char* ram_file);
int save_ram();

#endif