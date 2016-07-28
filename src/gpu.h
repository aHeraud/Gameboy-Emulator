#ifndef GPU_H
#define GPU_H

//TODO: rename gpu to ppu

#include<stdint.h>

#define GPU_MODE_HBLANK  0
#define GPU_MODE_VBLANK  1
#define GPU_MODE_SEARCH_OAM  2
#define GPU_MODE_TRANSFER  3

typedef struct {
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
} pixel_t;

typedef struct {
	pixel_t screen[160 * 144];
	uint16_t clock;
} gpu_t;

extern gpu_t gpu;

typedef struct {
	uint8_t y;
	uint8_t x;
	uint8_t tile_number;
	uint8_t options;
} sprite_t;

void gpu_init();
void gpu_reset();
void gpu_step();
void gpu_render_scanline();
void gpu_clearln(uint8_t line);

#endif