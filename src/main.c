#include<stdio.h>
#include<stdbool.h>
#include<stdint.h>
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

	emulator_is_running = true;

    //TODO: this doesn't work, need better method to synchronize emulator
	const uint64_t CPU_FREQ = 4194304;	//DMG cpu freq in Hz
	uint64_t freq = SDL_GetPerformanceFrequency();	//Frequency of the PerformanceCounter in Hz
	double step_time = (double)freq / CPU_FREQ;
    uint64_t last, now;

	last = SDL_GetPerformanceCounter();
    while (emulator_is_running) {
		now = SDL_GetPerformanceCounter();
		uint64_t delta = now - last;
		while(delta > step_time) {
			emulator_step();
			delta -= step_time;
		}
		last = now + delta;
	}
	
	return 0;
}

