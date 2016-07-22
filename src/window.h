#ifndef WINDOW_H
#define WINDOW_H

#include<SDL.h>

typedef struct {
	SDL_Window *id;
	SDL_Surface *screen;
} window_t;

extern window_t window;
extern window_t vram_view;

//Call this after gpu_init()
int window_init(const char *name, int width, int height);

void window_update();

void window_destroy();

#endif