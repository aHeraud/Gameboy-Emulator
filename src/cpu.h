#ifndef CPU_H
#define CPU_H

#include<stdint.h>
#include<stdbool.h>

extern const uint8_t VBLANK_INTERRUPT;
extern const uint8_t LCD_STAT_INTERRUPT;
extern const uint8_t TIMER_INTERRUPT;
extern const uint8_t SERIAL_INTERRUPT;
extern const uint8_t JOYPAD_INTERRUPT;

typedef struct {
	//TODO: this is byte order dependent, to build on arm the order needs to be reversed
	union {	//AF
		struct {
			uint8_t f;
			uint8_t a;
		};
		uint16_t af;
	};

	union {	//BC
		struct {
			uint8_t c;
			uint8_t b;
		};
		uint16_t bc;
	};

	union {	//ED
		struct {
			uint8_t e;
			uint8_t d;
		};
		uint16_t de;
	};

	union {	//HL
		struct {
			uint8_t l;
			uint8_t h;
		};
		uint16_t hl;
	};

	union { //SP
		struct {
			uint8_t sp_low;
			uint8_t sp_high;
		};
		uint16_t sp;
	};

	union { //SP
		struct {
			uint8_t pc_low;
			uint8_t pc_high;
		};
		uint16_t pc;
	};

} registers_t;

typedef struct {
	registers_t registers;
	bool ime;
	bool halt, stop;
	unsigned int cycle_counter;
} cpu_t;

extern cpu_t cpu;

/**/
void cpu_step();

/**
	Resets cpu
*/
void cpu_reset();
void cpu_init();

//Push a word on to the stack
void push(uint16_t);

//Pop a word off of the stack
uint16_t pop();

void request_interrupt(uint8_t interrupt_mask);
bool interrupt_service_routine();

#endif