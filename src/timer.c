#include<stdbool.h>

#include"timer.h"
#include"cpu.h"

/*
Timer Registers:
FF04 (DIV): Incremented at a rate of 16384Hz, writing to it resets it to 0.
FF05 (TIMA): Incremented by a clock frequency specified by TAC.
	When it overflows, it is set to the value in TAC, and 
	request a timer interrupt.
FF06 (TMA): Data to load into TIMA when it overflows.
FF07 (TAC): Controls TIMA, bit 2 = enable, bit 1,0 = speed select.
*/

//TODO: implement obscure behavior referenced in http://gbdev.gg8.se/wiki/articles/Timer_Obscure_Behaviour

const uint16_t timer_freq[4] = {1024, 16, 64, 256};
uint16_t timer_div;

void timer_reset() {
	timer_div = 0;
}

void timer_update() {
	timer_div += 4;	//update is only called every 4 cycles
	uint8_t tac = memory.io[0x07];

	if (tac & 4) {
		//Timer is enabled
		if(timer_div % timer_freq[tac & 3] == 0) {
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