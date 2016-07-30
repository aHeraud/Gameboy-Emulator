#include<stdio.h>
#include<stdbool.h>
#include<stdint.h>
#include<string.h>
#include<SDL.h>

#include"mmu.h"
#include"emulator.h"

int main(int argc, char** argv) {
	const char* rom_file = argv[1];
	if (argc < 2) {
		printf("Invalid Arguments: pass name of a rom to play.\n");
		return 0;
	}

	emulator_init();
	if (load_rom(rom_file)) {
		printf("Error: Failed to read rom\n");
		return 1;
	}

	//try to see if there is a save
	size_t len = strlen(rom_file) + 5;	//leave room for ".sav\0"
	char save[len];
	memset(save, 0, len);
	memcpy(save, rom_file, strlen(rom_file));
	char* ptr = strrchr(save, '.');
	if(!ptr) {
		ptr = save + len - 5;
		save[len - 5] = '.';
	}
	memcpy(ptr + 1, "sav\0", 4);
	load_ram(save);

	emulator_is_running = true;

	const uint64_t CPU_FREQ = 4194304 / 4;
	uint64_t freq = SDL_GetPerformanceFrequency();
	double step_time = ((double)freq / CPU_FREQ);
    uint64_t last, now;

	last = SDL_GetPerformanceCounter();
    while (emulator_is_running) {
		now = SDL_GetPerformanceCounter();
		uint64_t delta = now - last;
		while(delta > step_time) {
			emulator_step();
			delta -= step_time;
			last += step_time;
		}
	}
	
	return 0;
}

