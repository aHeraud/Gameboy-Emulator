#include<stdint.h>
#include<stdio.h>
#include<stdbool.h>
#include<SDL.h>

#include"mmu.h"
#include"emulator.h"

/*
Tests:
01-special: Passed
02-interrupts: Timer doesn't work failed #4
03-op sp,hl: Passed
04-op r,imm: Passed
05-op rp: Passed
06-ld r,r: Passed
07-jr,jp,call,ret,rst: Passed
08-misc instrs: Passed
09-op r,r: Passed
10-bit ops: Passed
11-op a,(hl): Passed
*/

int main(int argc, char** argv) {
	//const char* rom_file = argv[1];
	const char* rom_file = "test_roms/cpu_instrs/02-interrupts.gb";
	if (argc < 2) {
		printf("Invalid Arguments: pass name of a rom to play.\n");
		return 0;
	}

	emulator_init();
	if (load_rom(rom_file)) {
		printf("Error: Failed to read rom\n");
		return 1;
	}

	emulator_is_running = true;

	while (emulator_is_running) {
			emulator_step();
	}
	
	return 0;
}

