#include"joypad.h"
#include"mmu.h"
#include"cpu.h"

joypad_t joypad;

const SDL_Keycode joypad_default_keybindings[8] = { SDLK_UP, SDLK_DOWN, SDLK_LEFT, SDLK_RIGHT, SDLK_c, SDLK_v, SDLK_z, SDLK_x };

void joypad_init() {
	for (int i = 0; i < 8; ++i)
		joypad.keys[i] = false;
}

void joypad_update() {
	//Read joypad register to see what buttons are selected
	uint8_t select = read_byte(0xFF00) & 0x30;	//Clear everything but select bits
	uint8_t joyp = 0;
	if (select & 0x10) {
		//Buttons selected
		if (!joypad.keys[JOYPAD_START]) joyp |= 0b1000;
		if (!joypad.keys[JOYPAD_SELECT]) joyp |= 0b0100;
		if (!joypad.keys[JOYPAD_B]) joyp |= 0b0010;
		if (!joypad.keys[JOYPAD_A]) joyp |= 0b0001;
	}
	else {
		//Direction keys selected
		if (!joypad.keys[JOYPAD_DOWN]) joyp |= 0b1000;
		if (!joypad.keys[JOYPAD_UP]) joyp |= 0b0100;
		if (!joypad.keys[JOYPAD_LEFT]) joyp |= 0b0010;
		if (!joypad.keys[JOYPAD_RIGHT]) joyp |= 0b0001;
	}

	joyp |= select;
	write_byte(0xFF00, joyp);
}

void joypad_keydown(SDL_Keycode key) {
	//TODO add rebindable controls and get rid of if statements
	uint8_t select = read_byte(0xFF00) & 0x30;
	if (((select & 0x10) == 0) && (key == joypad_default_keybindings[JOYPAD_UP] || key == joypad_default_keybindings[JOYPAD_DOWN] || key == joypad_default_keybindings[JOYPAD_LEFT] || key == joypad_default_keybindings[JOYPAD_RIGHT]))
		request_interrupt(JOYPAD_INTERRUPT);
	else if(((select & 0x20) == 0) && (key == joypad_default_keybindings[JOYPAD_A] || key == joypad_default_keybindings[JOYPAD_B] || key == joypad_default_keybindings[JOYPAD_SELECT] || key == joypad_default_keybindings[JOYPAD_START]))
		request_interrupt(JOYPAD_INTERRUPT);

	if (key == joypad_default_keybindings[JOYPAD_UP]) joypad.keys[JOYPAD_UP] = true;
	else if (key == joypad_default_keybindings[JOYPAD_DOWN]) joypad.keys[JOYPAD_DOWN] = true;
	else if (key == joypad_default_keybindings[JOYPAD_LEFT]) joypad.keys[JOYPAD_LEFT] = true;
	else if (key == joypad_default_keybindings[JOYPAD_RIGHT]) joypad.keys[JOYPAD_RIGHT] = true;
	else if (key == joypad_default_keybindings[JOYPAD_A]) joypad.keys[JOYPAD_A] = true;
	else if (key == joypad_default_keybindings[JOYPAD_B]) joypad.keys[JOYPAD_B] = true;
	else if (key == joypad_default_keybindings[JOYPAD_SELECT]) joypad.keys[JOYPAD_SELECT] = true;
	else if (key == joypad_default_keybindings[JOYPAD_START]) joypad.keys[JOYPAD_START] = true;
}

void joypad_keyup(SDL_Keycode key) {
	//TODO add rebindable controls and get rid of if statements
	if (key == joypad_default_keybindings[JOYPAD_UP]) joypad.keys[JOYPAD_UP] = false;
	else if (key == joypad_default_keybindings[JOYPAD_DOWN]) joypad.keys[JOYPAD_DOWN] = false;
	else if (key == joypad_default_keybindings[JOYPAD_LEFT]) joypad.keys[JOYPAD_LEFT] = false;
	else if (key == joypad_default_keybindings[JOYPAD_RIGHT]) joypad.keys[JOYPAD_RIGHT] = false;
	else if (key == joypad_default_keybindings[JOYPAD_A]) joypad.keys[JOYPAD_A] = false;
	else if (key == joypad_default_keybindings[JOYPAD_B]) joypad.keys[JOYPAD_B] = false;
	else if (key == joypad_default_keybindings[JOYPAD_SELECT]) joypad.keys[JOYPAD_SELECT] = false;
	else if (key == joypad_default_keybindings[JOYPAD_START]) joypad.keys[JOYPAD_START] = false;
}
