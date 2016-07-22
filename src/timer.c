#include"timer.h"
#include"cpu.h"

/*
Timer Registers:
FF04 (DIV): Incremented at a rate of 16384Hz, writing to it resets it to 0. (TODO: reset on write)
FF05 (TIMA): Incremented by a clock frequency specified by TAC.
	When it overflows, it is set to the value in TAC, and 
	request a timer interrupt.
FF06 (TMA): Data to load into TIMA when it overflows.
FF07 (TAC): Controls TIMA, bit 2 = enable, bit 1,0 = speed select.
*/

uint32_t timer_counter;

void timer_reset() {
	timer_counter = 0;
}

void timer_update() {
	timer_counter += 1;
	uint8_t tac = memory.io[0x07];

	if (timer_counter % 256 == 0) {
		//Div is incremented at 1/256 speed of cpu clock
		memory.io[4] += 1;
	}

	if (tac & 4) {
		//Timer is enabled
		tac &= 3;
		uint8_t did_increment = 0;
		switch (tac) {
		case 0:
			//clock/1024
			if (timer_counter % 1024 == 0) {
				did_increment = 1;
			}
			break;

		case 1:
			//clock/16
			if (timer_counter % 16 == 0) {
				did_increment = 1;
			}
			break;

		case 2:
			//clock/64
			if (timer_counter % 64 == 0) {
				did_increment = 1;
			}
				
			break;

		case 3:
			//clock/256
			if (timer_counter % 256 == 0) {
				did_increment = 1;
			}
			break;
		}

		if (did_increment) {
			memory.io[0x05] += 1;
			if (memory.io[0x05] == 0) {
				//Timer overflowed
				//Load tma into tima, and request timer interrupt
				memory.io[0x05] = memory.io[0x06];
				request_interrupt(TIMER_INTERRUPT);
			}
		}
	}

}