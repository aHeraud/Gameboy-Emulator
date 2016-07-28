#include"cpu.h"
#include"mmu.h"
#include"instructions.h"
#include"serial.h"
#include"joypad.h"
#include"timer.h"

#include<assert.h>

cpu_t cpu;

const uint8_t VBLANK_INTERRUPT		= 1;
const uint8_t LCD_STAT_INTERRUPT	= 2;
const uint8_t TIMER_INTERRUPT		= 4;
const uint8_t SERIAL_INTERRUPT		= 8;
const uint8_t JOYPAD_INTERRUPT		= 16;

void cpu_init() {
	cpu_reset();
}

void cpu_step() {

	timer_update();
	serial_update();
	joypad_update();

	if (cpu.cycle_counter) {
		cpu.cycle_counter -= 1;
		return;
	}

	if (interrupt_service_routine())
		return;

	if (cpu.halt)
		return;

	/*Read next opcode*/
	uint8_t opcode;
	//uint8_t prefix = 0;
	uint8_t length = 0;
	uint8_t op8 = 0;
	uint16_t op16 = 0;

	opcode = read_byte(cpu.registers.pc++);
	if (opcode == 0xCB) {
		//prefix = opcode;
		opcode = read_byte(cpu.registers.pc++);
		void(*execute)(void) = extended_instructions[opcode].func;
		execute();
		cpu.cycle_counter += extended_instruction_cycles[opcode];
	}
	else {
		length = instructions[opcode].length;
		if (length == 1) {
			void(*execute)(void) = instructions[opcode].func;
			execute();
		}
		else if (length == 2) {
			op8 = read_byte(cpu.registers.pc++);
			void(*execute)(uint8_t) = instructions[opcode].func;
			execute(op8);
		}
		else if (length == 3) {
			uint8_t low = read_byte(cpu.registers.pc++);
			uint8_t high = read_byte(cpu.registers.pc++);
			op16 = (high << 8) | low;
			void(*execute)(uint16_t) = instructions[opcode].func;
			execute(op16);
		}
		else {
			//Invalid
			assert(false);
		}
		cpu.cycle_counter += instruction_cycles[opcode];	//Do this after the instruction executes because they have variable timings

	}
}

void cpu_reset() {
	cpu.cycle_counter = 0;

	cpu.registers.af = 0x01B0;
	cpu.registers.bc = 0x0013;
	cpu.registers.de = 0x00D8;
	cpu.registers.hl = 0x014D;
	cpu.registers.sp = 0xFFFE;
	cpu.registers.pc = 0x0100;
}

//Push a word on to the stack
void push(uint16_t op16) {
	uint8_t low = op16 & 255;
	uint8_t high = (op16 >> 8) & 255;
	write_byte(cpu.registers.sp - 1, high);
	write_byte(cpu.registers.sp - 2, low);
	cpu.registers.sp -= 2;
}

//Pop a word off of the stack
uint16_t pop() {
	uint8_t low;
	uint8_t high;
	low = read_byte(cpu.registers.sp);
	high = read_byte(cpu.registers.sp + 1);
	cpu.registers.sp += 2;
	uint16_t val = (high << 8) | low;
	return val;
}

void request_interrupt(uint8_t interrupt_mask) {
	//Requesting an interrupt disables halt, it doesn't actually have to proccess the  interrupts
	cpu.halt = false;

	uint8_t requested_interrupts = read_byte(0xFF0F);
	requested_interrupts |= interrupt_mask;
	requested_interrupts &= 0x1F;
	write_byte(0xFF0F, requested_interrupts);
}

bool interrupt_service_routine() {
	//According to http://gbdev.gg8.se/wiki/articles/Interrupts,
	//the isr should consume a total of 5 machine cycles
	//When an enabled interrupt is requested, the cpu clears the flag and does an action similar
	//to DI; CALL(<INT>)
	//TODO: Add masks for interrupts
	if (cpu.ime) {
		uint8_t requested_interrupts = ((read_byte(0xFFFF) & 0x1F) & (read_byte(0xFF0F) & 0x1F));
		if (!requested_interrupts)	return false;

		cpu.halt = false;
		cpu.ime = false;    //Nested interrupts have to be enabled by an EI instruction inside the current interrupt vector
		push(cpu.registers.pc);

		if (requested_interrupts & VBLANK_INTERRUPT) {
			//VBLANK
			write_byte(0xFF0F, (requested_interrupts & (~VBLANK_INTERRUPT)));	//Clear VBlank interrupt request bit
			cpu.registers.pc = 0x40;
		}

		else if (requested_interrupts & LCD_STAT_INTERRUPT) {
			//LCD STAT
			write_byte(0xFF0F, (requested_interrupts & (~LCD_STAT_INTERRUPT)));	//Clear VBlank interrupt request bit
			cpu.registers.pc = 0x48;
		}

		else if (requested_interrupts & TIMER_INTERRUPT) {
			//TIMER
			write_byte(0xFF0F, (requested_interrupts & (~TIMER_INTERRUPT)));	//Clear VBlank interrupt request bit
			cpu.registers.pc = 0x50;
		}

		else if (requested_interrupts & SERIAL_INTERRUPT) {
			//Serial
			write_byte(0xFF0F, (requested_interrupts & (~SERIAL_INTERRUPT)));	//Clear VBlank interrupt request bit
			cpu.registers.pc = 0x58;
		}

		else if (requested_interrupts & JOYPAD_INTERRUPT) {
			//Joypad
			write_byte(0xFF0F, (requested_interrupts & (~JOYPAD_INTERRUPT)));	//Clear VBlank interrupt request bit
			cpu.registers.pc = 0x60;
		}

		cpu.cycle_counter = 5;
		return true;
	}
	else {
		return false;
	}
}