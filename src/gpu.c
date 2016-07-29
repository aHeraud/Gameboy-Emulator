#include"gpu.h"

#include<stdint.h>
#include<string.h>

#include"mmu.h"
#include"cpu.h"
#include"window.h"


/*
Using the Pan Docs as a reference
http://gbdev.gg8.se/wiki/articles/Video_Display
*/

gpu_t gpu;

const pixel_t bg_shades[4] = { { 255, 255, 255, 255 }, {170, 170, 170, 255},{ 85, 85, 85, 255 }, { 0, 0, 0, 255 } };
const pixel_t sprite_shades[4] = { { 255, 255, 255, 255 },{ 170, 170, 170, 255 },{ 85, 85, 85, 255 },{ 0, 0, 0, 255 } };

const uint8_t COINCIDENCE_INTERRUPT_ENABLE_MASK = 64;
const uint8_t OAM_INTERRUPT_ENABLE_MASK = 32;
const uint8_t VBLANK_INTERRUPT_ENABLE_MASK = 16;
const uint8_t HBLANK_INTERRUPT_ENABLE_MASK = 8;

void draw_bg();
void draw_sprites(int layer);
void draw_sprite(sprite_t sprite, uint8_t line, uint8_t height);

void gpu_init() {
	gpu_reset();
}

void gpu_reset() {
	gpu.clock = 0;
	memset(gpu.screen, 0, 160 * 144 * sizeof(pixel_t));

	//Set video registers to the value they are at the end of the boot rom
	memory.io[0x40] = 0x91;	//LCDC
	memory.io[0x41] = 0x85;	//LCD STAT
	memory.io[0x42] = 0x00;	//SCY
	memory.io[0x43] = 0x00;	//SCX
	memory.io[0x44] = 0x00;	//LY	TODO: What is value of ly at the end of the bootrom?
	memory.io[0x45] = 0x00;	//LYC
	memory.io[0x46] = 0xFF;	//??
	memory.io[0x47] = 0xFC;	//BGP
	memory.io[0x48] = 0xFF;	//OBP0
	memory.io[0x49] = 0xFF;	//OBP1
	memory.io[0x4A] = 0x00;	//WY
	memory.io[0x4B] = 0x00;	//WX
}

/*
Mode 0 = H-Blank
Mode 1 = V-Blank
Mode 2 = Searching OAM
Mode 3 = Transferring Data to LCD Driver
timings for the different modes from Imran Nazars javascript emulator
http://imrannazar.com/GameBoy-Emulation-in-JavaScript:-GPU-Timings
*/
void gpu_step() {
	uint8_t lcdc = memory.io[0x40];	//LCDC (LCD Control)
	uint8_t stat = memory.io[0x41];	//Read STAT (LCDC STATUS) register
	uint8_t mode = stat & 3;		//Read GPU Mode from low 2 bits of LCDC STATUS register
	uint8_t line = memory.io[0x44];	//Read line from LY (LCDC Y-Coordinate) register

	if (!(lcdc & 128))
		return;	//LCD is disabled

	stat = stat & 0b01111000;	//Clear coincidence flag and mode

	//gpu.clock += 1;
	gpu.clock += 4;	//Called every 4 clock cycles, all timings should be multiples of 4

	/**/
	switch (mode) {
	case GPU_MODE_HBLANK:	//H-Blank
		if (gpu.clock > 228) {
			line += 1;
			gpu.clock = 0;

			if (line >= 144) {
				mode = GPU_MODE_VBLANK;

				//VBlank can trigger both VBlank and LCD STAT interrupts
				//http://gameboy.mongenel.com/dmg/istat98.txt


				request_interrupt(VBLANK_INTERRUPT);
				if(stat & VBLANK_INTERRUPT_ENABLE_MASK)
					request_interrupt(LCD_STAT_INTERRUPT);

				//Draw frame onto screen
				window_update();
			}
			else {
				mode = GPU_MODE_SEARCH_OAM;
				if (stat & OAM_INTERRUPT_ENABLE_MASK)
					request_interrupt(LCD_STAT_INTERRUPT);
			}
		}
		break;

	case GPU_MODE_VBLANK:	//V-Blank
		if (gpu.clock > 456) {	//4560 / 10
			line += 1;
			gpu.clock = 0;
			if (line >= 153) {
				line = 0;
				mode = GPU_MODE_SEARCH_OAM;
				if (stat & OAM_INTERRUPT_ENABLE_MASK)
					request_interrupt(LCD_STAT_INTERRUPT);
			}
		}
		break;

	case GPU_MODE_SEARCH_OAM:	//Reading OAM
		if (gpu.clock > 76) {
			gpu.clock = 0;
			mode = GPU_MODE_TRANSFER; //I don't think that this triggers an interrupt
		}
		break;

	case GPU_MODE_TRANSFER:	//Reading from OAM and VRAM
		if (gpu.clock > 152) {
			mode = GPU_MODE_HBLANK;
			gpu.clock = 0;
			if (stat & HBLANK_INTERRUPT_ENABLE_MASK)
				request_interrupt(LCD_STAT_INTERRUPT);
			gpu_render_scanline();
		}
		break;
	}

	//Check LYC=LY Coincidence interrupt (already cleared the flag from last tick)
	if(memory.io[0x44] == memory.io[0x45]) {
		stat |= 4;
		if((stat & COINCIDENCE_INTERRUPT_ENABLE_MASK))
			request_interrupt(LCD_STAT_INTERRUPT);
	}


	//Write changes to memory
	stat = stat | mode;
	memory.io[0x41] = stat;
	memory.io[0x44] = line;
}

void gpu_render_scanline() {
	gpu_clearln(memory.io[0x44]);
	draw_sprites(0);
	draw_bg();
	draw_sprites(1);
}

void draw_bg() {
	/*First BG/Window*/
	uint8_t lcdc = memory.io[0x40];
	uint8_t line = memory.io[0x44];	//Current scanline
	uint8_t yScroll = memory.io[0x42];
	uint8_t xScroll = memory.io[0x43];

	uint8_t bgp = memory.io[0x47];
	uint16_t bg_tile_map = 0;
	uint16_t tile_data = 0;

	//Tile Map (get tile number here)
	bg_tile_map = (lcdc & 8) ? 0x9C00 : 0x9800;
	//Tile data start (read tile data here)
	tile_data = (lcdc & 16) ? 0x8000 : 0x8800;
	uint8_t yPos = line + yScroll;

	//Setup Window
	bool window_enabled;
	uint8_t wy = memory.io[0x4A];		//Window Y position
	uint8_t wx = memory.io[0x4B] - 7;	//Window X position
	uint16_t window_tile_map = (lcdc & 64) ? 0x9C00 : 0x9800;
	window_enabled = (lcdc & 32) && (wy <= line);	
		//Check if window display enable is set and the
		//top of the window is on or above this line.


	for (uint8_t x = 0; x < 160; ++x) {
		uint8_t xPos = x + xScroll;
		uint16_t tile_address = 0;
		uint16_t map_address = 0;
		uint8_t tile_number = 0;

		if (window_enabled && (x >= wx)) {
			map_address = window_tile_map + ((x - wx) / 8) + (((line - wy) / 8) * 32);
			//Window doesn't scroll
			xPos = x;
			yPos = line;
		}
		else if(lcdc & 1) {
			//BG enabled
			map_address = bg_tile_map + (xPos / 8) + ((yPos / 8) * 32);
		}
		else {
			//BG and Window Disabled
			continue;
		}
		
		tile_number = read_byte(map_address);
		if (tile_data == 0x8800) {
			//Convert from signed tile numbers into unsigned offset
			tile_number += 128;
		}

		//Read tile
		tile_address = tile_data + (tile_number * 16) + ((yPos % 8) * 2);
		uint8_t tile_2 = read_byte(tile_address);
		uint8_t tile_1 = read_byte(tile_address + 1);

		uint8_t value = ((tile_1 >> (7 - (xPos % 8)) << 1) & 2) | ((tile_2 >> (7 - (xPos % 8))) & 1);	//Color value for current pixel
		uint8_t shade = (bgp >> (value << 1)) & 3;
		pixel_t color = bg_shades[shade];

		if (shade || gpu.screen[x + (line * 160)].a == 0)
			gpu.screen[x + (line * 160)] = color;


		
		
	}
}

void draw_sprites(int desired_layer) {
	uint8_t lcdc = memory.io[0x40];	//LCDC Register
	uint8_t line = memory.io[0x44];
	if (!(lcdc & 2))
		return;	//sprite display disabled

	uint8_t height = (lcdc & 4) ? 16 : 8;
	uint8_t sprite_order[40] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,
		20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39 };
	//TODO: Sort by x value (descending)

	for (int i = 0; i < 40; ++i) {
		sprite_t sprite;
		memcpy(&sprite, &memory.oam[sprite_order[i] * 4], sizeof(sprite_t));
		sprite.y -= 16;
		sprite.x -= 8;
		if (height == 16)
			sprite.tile_number &= 0xFE;

		if (sprite.y == 0 || sprite.y >= 160 || sprite.x == 0 || sprite.x >= 168)
			continue;	//Sprite is completely off screen

		if (sprite.y > line || sprite.y + height < line)
			continue;	//Sprite doesn't intersect current scanline

		uint8_t layer = sprite.options & 128;
		
		if (desired_layer == 0) {
			if (layer)
				draw_sprite(sprite, line, height);
		}

		else if (desired_layer) {
			if (!layer)
				draw_sprite(sprite, line, height);
		}
	}
}

inline void draw_sprite(sprite_t sprite, uint8_t line, uint8_t height) {
	//TODO: optomise sprite code
	bool x_flip = sprite.options & 0x20 ? true : false;
	bool y_flip = sprite.options & 0x40 ? true : false;
	uint16_t tile_address = sprite.tile_number * 16 + 0x8000;
	uint16_t lower_tile_address = (sprite.tile_number | 1) * 16 + 0x8000;
	uint8_t palette = read_byte((sprite.options & 0x10) ? 0xFF49 : 0xFF48);	//Remember low 2 bits are not used (transparent)
	
	int y = line - sprite.y;
	if (y < 0 || y >= height)
		return;	//not on this line

	uint8_t data0;
	uint8_t data1;

	if (y >= 8) tile_address = lower_tile_address;

	if (y_flip) {
		data0 = read_byte(tile_address + 1 + (((height - y) % 8) * 2));
		data1 = read_byte(tile_address + (((height - y) % 8) * 2));
	}
	else {
		data0 = read_byte(tile_address + 1 + ((y % 8) * 2));
		data1 = read_byte(tile_address + ((y % 8) * 2));
	}

	for (int x = 0; x < 8; ++x) {
		if (x + sprite.x >= 160)
			continue;	//This pixel is out of bounds

		//Draw sprite
		uint8_t value;
		if (x_flip) {
			value = ((data0 >> (x % 8) << 1) & 2) | ((data1 >> (x % 8)) & 1);	//Color value for current pixel
		}
		else {
			value = ((data0 >> (7 - (x % 8)) << 1) & 2) | ((data1 >> (7 - (x % 8))) & 1);	//Color value for current pixel
		}
		uint8_t shade = (uint8_t)((palette >> (value << 1)) & 3);
		pixel_t color = sprite_shades[shade];

		if(value)
			gpu.screen[x + sprite.x + (line * 160)] = color;
	}
}

void gpu_clearln(uint8_t line) {
	//Clear the current scanline
	memset(gpu.screen + (160 * line), 0, 160 * sizeof(pixel_t));
}