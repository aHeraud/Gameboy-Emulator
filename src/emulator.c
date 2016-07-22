#include"emulator.h"
#include"cpu.h"
#include"gpu.h"
#include"mmu.h"
#include"window.h"
#include"serial.h"
#include"timer.h"
#include"joypad.h"

bool emulator_is_running;

void emulator_init() {
	gpu_init();
	mmu_init();
	cpu_init();
	joypad_init();
	window_init("Gameboy Emulator", 320, 288);
	timer_reset();
	serial_reset();
}

void emulator_reset() {
	mmu_reset();
	cpu_reset();
	gpu_reset();
	timer_reset();
	serial_reset();
}

void emulator_step() {
	cpu_step();
	gpu_step();
}

void emulator_stop() {
	//TODO: Save ram to file (if battery back up)
	//save_ram();

	//Clean Up
	//Destroy SDL window and mmu
	window_destroy();
	mmu_destroy();

	//Quit
	SDL_Quit();

	emulator_is_running = false;
}