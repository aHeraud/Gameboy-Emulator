#include<stdint.h>
#include<stdio.h>

#include"serial.h"
#include"mmu.h"
#include"cpu.h"

void serial_reset() {
	//Set SB and SC
	memory.io[1] = 0;
	memory.io[2] = 0;
}

/* Used to write bytes from the serial port to the console for blargg's cpu test roms */
void serial_update() {
		
	uint8_t sc = memory.io[2];
	uint8_t sb = memory.io[1];

	if ((sc & 129) == 129) {	//Both transfer start and internal clock
		memory.io[2] = 0;
		memory.io[1] = 0;

		//Transfer start flag set
		printf("%c", sb);

		//request serial interrupt
		request_interrupt(SERIAL_INTERRUPT);
	}
}