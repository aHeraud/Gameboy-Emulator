#ifndef JOYPAD_H
#define JOYPAD_H

#include<stdint.h>
#include<stdbool.h>

#include<SDL_keyboard.h>
#include<SDL_gamecontroller.h>

#define JOYPAD_UP 0
#define JOYPAD_DOWN 1
#define JOYPAD_LEFT 2
#define JOYPAD_RIGHT 3
#define JOYPAD_SELECT 4
#define JOYPAD_START 5
#define JOYPAD_A 6
#define JOYPAD_B 7

extern const SDL_Keycode joypad_default_keybindings[8];

typedef struct {
	bool keys[8]; //Whether each button is pressed
	SDL_Keycode keybindings[8];	//TODO
} joypad_t;

extern joypad_t joypad;

void joypad_init();

//keyboards
void joypad_keydown(SDL_Keycode key);
void joypad_keyup(SDL_Keycode key);

//controllers
void joypad_buttondown(SDL_GameControllerButton button);
void joypad_buttonup(SDL_GameControllerButton button);

void joypad_update();

#endif