#include"window.h"
#include"gpu.h"
#include"joypad.h"
#include"emulator.h"

window_t window;

/**
Returns 0 for success, 1 for error
*/
int window_init(const char *name, int width, int height) {
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		return 1;

	window.id = SDL_CreateWindow(name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, 0);
	if (window.id == NULL)
		return 1;

	//I think SDL might read them as dwords, so endianness matters for the mask
	window.screen = SDL_CreateRGBSurfaceFrom(gpu.screen, 160, 144, 32, (160 * 4), 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
	if (window.screen == NULL)
		return 1;
	return 0;
}

void window_update() {
	SDL_BlitScaled(window.screen, NULL, SDL_GetWindowSurface(window.id), NULL);

	SDL_UpdateWindowSurface(window.id);

	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_KEYDOWN:
			joypad_keydown(event.key.keysym.sym);
			break;
		case SDL_KEYUP:
			joypad_keyup(event.key.keysym.sym);
			break;
		case  SDL_QUIT:
			//exit
			emulator_stop();
			break;

		default:
			break;
		}
	}
}

void window_destroy() {
	SDL_DestroyWindow(window.id);
	SDL_FreeSurface(window.screen);
	//SDL_Quit();
}