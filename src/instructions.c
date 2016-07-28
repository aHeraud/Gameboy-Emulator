#include"instructions.h"
#include"cpu.h"
#include"mmu.h"
#include"register8.h"
#include"register16.h"

//Instruction cycle tables from the readme of blargg's instr_timing test rom
uint8_t instruction_cycles[256] = {
		1,3,2,2,1,1,2,1,5,2,2,2,1,1,2,1,
		0,3,2,2,1,1,2,1,3,2,2,2,1,1,2,1,
		2,3,2,2,1,1,2,1,2,2,2,2,1,1,2,1,
		2,3,2,2,3,3,3,1,2,2,2,2,1,1,2,1,
		1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,
		1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,
		1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,
		2,2,2,2,2,2,0,2,1,1,1,1,1,1,2,1,
		1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,
		1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,
		1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,
		1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,
		2,3,3,4,3,4,2,4,2,4,3,0,3,6,2,4,
		2,3,3,0,3,4,2,4,2,4,3,0,3,0,2,4,
		3,3,2,0,0,4,2,4,4,1,4,0,0,0,2,4,
		3,3,2,1,0,4,2,4,3,2,4,1,0,0,2,4
};

const uint8_t instruction_cycles_when_conditionals_not_taken[256] = {
		1,3,2,2,1,1,2,1,5,2,2,2,1,1,2,1,
		0,3,2,2,1,1,2,1,3,2,2,2,1,1,2,1,
		2,3,2,2,1,1,2,1,2,2,2,2,1,1,2,1,
		2,3,2,2,3,3,3,1,2,2,2,2,1,1,2,1,
		1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,
		1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,
		1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,
		2,2,2,2,2,2,0,2,1,1,1,1,1,1,2,1,
		1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,
		1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,
		1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,
		1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,
		2,3,3,4,3,4,2,4,2,4,3,0,3,6,2,4,
		2,3,3,0,3,4,2,4,2,4,3,0,3,0,2,4,
		3,3,2,0,0,4,2,4,4,1,4,0,0,0,2,4,
		3,3,2,1,0,4,2,4,3,2,4,1,0,0,2,4
};

const uint8_t instruction_cycles_when_conditionals_taken[] = {
		1,3,2,2,1,1,2,1,5,2,2,2,1,1,2,1,
		0,3,2,2,1,1,2,1,3,2,2,2,1,1,2,1,
		3,3,2,2,1,1,2,1,3,2,2,2,1,1,2,1,
		3,3,2,2,3,3,3,1,3,2,2,2,1,1,2,1,
		1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,
		1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,
		1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,
		2,2,2,2,2,2,0,2,1,1,1,1,1,1,2,1,
		1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,
		1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,
		1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,
		1,1,1,1,1,1,2,1,1,1,1,1,1,1,2,1,
		5,3,4,4,6,4,2,4,5,4,4,0,6,6,2,4,
		5,3,4,0,6,4,2,4,5,4,4,0,6,0,2,4,
		3,3,2,0,0,4,2,4,4,1,4,0,0,0,2,4,
		3,3,2,1,0,4,2,4,3,2,4,1,0,0,2,4
};

/*Regular Instructions*/
const instruction_t instructions[256] = {
	{0x00, "NOP\0",	1, nop},
	{0x01, "LD BC, %04X\0", 3, ld_bc_d16},
	{0x02, "LD (BC), A\0", 1, ld_at_bc_a},
	{0x03, "INC BC\0", 1, inc_bc},
	{0x04, "INC B\0", 1, inc_b},
	{0x05, "DEC B\0", 1, dec_b},
	{0x06, "LD B, %02X\0", 2, ld_b_d8},
	{0x07, "RLCA\0", 1, rlca},
	{0x08, "LD (a16), SP\0", 3, ld_at_a16_sp},
	{0x09, "ADD HL, BC\0", 1, add_hl_bc},
	{0x0A, "LD A, (BC)\0", 1, ld_a_at_bc},
	{0x0B, "DEC BC\0", 1, dec_bc},
	{0x0C, "INC C\0", 1, inc_c},
	{0x0D, "DEC C\0", 1, dec_c},
	{0x0E, "LD C, %02X\0", 2, ld_c_d8},
	{0x0F, "RRCA\0", 1, rrca},
	
	{0x10, "STOP\0", 1, stop},
	{0x11, "LD DE, %04X\0", 3, ld_de_d16},
	{0x12, "LD (DE), a\0", 1, ld_at_de_a},
	{0x13, "INC DE\0", 1, inc_de},
	{0x14, "INC D\0", 1, inc_d},
	{0x15, "DEC D\0", 1, dec_d},
	{0x16, "LD D, %02X\0", 2, ld_d_d8},
	{0x17, "RLA\0", 1, rla},
	{0x18, "JR %02X\0", 2, jr_r8},
	{0x19, "ADD HL, DE\0", 1, add_hl_de},
	{0x1A, "LD A, (DE)\0", 1, ld_a_at_de},
	{0x1B, "DEC DE\0", 1, dec_de},
	{0x1C, "INC E\0", 1, inc_e},
	{0x1D, "DEC E\0", 1, dec_e},
	{0x1E, "LD E, %02X\0", 2, ld_e_d8},
	{0x1F, "RRA\0", 1, rra},

	{0x20, "JR NZ, %02X\0", 2, jr_nz_r8},
	{0x21, "LD HL, %04X\0", 3, ld_hl_d16},
	{0x22, "LDI (HL), A\0", 1, ldi_at_hl_a},
	{0x23, "INC HL\0", 1, inc_hl},
	{0x24, "INC H\0", 1, inc_h},
	{0x25, "DEC H\0", 1, dec_h},
	{0x26, "LD H, %02X\0", 2, ld_h_d8},
	{0x27, "DAA\0", 1, daa},
	{0x28, "JR Z, %02X\0", 2, jr_z_r8},
	{0x29, "ADD HL, HL\0", 1, add_hl_hl},
	{0x2A, "LDI A, (HL)\0", 1, ldi_a_at_hl},
	{0x2B, "DEC HL\0", 1, dec_hl},
	{0x2C, "INC L\0", 1, inc_l},
	{0x2D, "DEC L\0", 1, dec_l},
	{0x2E, "LD L, %02X\0", 2, ld_l_d8},
	{0x2F, "CPL\0", 1, cpl},

	{0x30, "JR NC, %02X\0", 2, jr_nc_r8},
	{0x31, "LD SP, %04X\0", 3, ld_sp_d16},
	{0x32, "LDD (HL), A\0", 1, ldd_at_hl_a},
	{0x33, "INC SP\0", 1, inc_sp},
	{0x34, "INC (HL)\0", 1, inc_at_hl},
	{0x35, "DEC (HL)\0", 1, dec_at_hl},
	{0x36, "LD (HL), %02X\0", 2, ld_at_hl_d8},
	{0x37, "SCF\0", 1, scf},
	{0x38, "JR C, %02X\0", 2, jr_c_r8},
	{0x39, "ADD HL, SP\0", 1, add_hl_sp},
	{0x3A, "LDD A, (HL)\0", 1, ldd_a_at_hl},
	{0x3B, "DEC SP\0", 1, dec_sp},
	{0x3C, "INC A\0", 1, inc_a},
	{0x3D, "DEC A\0", 1, dec_a},
	{0x3E, "LD A, %02X\0", 2, ld_a_d8},
	{0x3F, "CCF\0", 1, ccf},

	{0x40, "LD B, B\0", 1, ld_b_b},
	{0x41, "LD B, C\0", 1, ld_b_c},
	{0x42, "LD B, D\0", 1, ld_b_d},
	{0x43, "LD B, E\0", 1, ld_b_e},
	{0x44, "LD B, H\0", 1, ld_b_h},
	{0x45, "LD B, L\0", 1, ld_b_l},
	{0x46, "LD B, (HL)\0", 1, ld_b_at_hl},
	{0x47, "LD B, A\0", 1,ld_b_a},
	{0x48, "LD C, B\0", 1, ld_c_b},
	{0x49, "LD C, C\0", 1, ld_c_c},
	{0x4A, "LC C, D\0", 1, ld_c_d},
	{0x4B, "LD C, E\0", 1, ld_c_e},
	{0x4C, "LD C, H\0", 1, ld_c_h},
	{0x4D, "LD C, L\0", 1, ld_c_l},
	{0x4E, "LD C, (HL)\0", 1, ld_c_at_hl},
	{0x4F, "LD C, A\0", 1, ld_c_a},

	{0x50, "LD D, B\0", 1, ld_d_b},
	{0x51, "LD D, C\0", 1, ld_d_c},
	{0x52, "LD D, D\0", 1, ld_d_d},
	{0x53, "LD D, E\0", 1, ld_d_e},
	{0x54, "LD D, H\0", 1, ld_d_h},
	{0x55, "LD D, L\0", 1, ld_d_l},
	{0x56, "LD D, (HL)\0", 1, ld_d_at_hl},
	{0x57, "LD D, A\0", 1,ld_d_a },
	{0x58, "LD E, B\0", 1, ld_e_b},
	{0x59, "LD E, C\0", 1, ld_e_c},
	{0x5A, "LC E, D\0", 1, ld_e_d},
	{0x5B, "LD E, E\0", 1, ld_e_e},
	{0x5C, "LD E, H\0", 1, ld_e_h},
	{0x5D, "LD E, L\0", 1, ld_e_l},
	{0x5E, "LD E, (HL)\0", 1, ld_e_at_hl},
	{0x5F, "LD E, A\0", 1, ld_e_a},

	{0x60, "LD H, B\0", 1, ld_h_b},
	{0x61, "LD H, C\0", 1, ld_h_c},
	{0x62, "LD H, D\0", 1, ld_h_d},
	{0x63, "LD H, E\0", 1, ld_h_e},
	{0x64, "LD H, H\0", 1, ld_h_h},
	{0x65, "LD H, L\0", 1, ld_h_l},
	{0x66, "LD H, (HL)\0", 1, ld_h_at_hl},
	{0x67, "LD H, A\0", 1,ld_h_a},
	{0x68, "LD L, B\0", 1, ld_l_b},
	{0x69, "LD L, C\0", 1, ld_l_c},
	{0x6A, "LC L, D\0", 1, ld_l_d},
	{0x6B, "LD L, E\0", 1, ld_l_e},
	{0x6C, "LD L, H\0", 1, ld_l_h},
	{0x6D, "LD L, L\0", 1, ld_l_l},
	{0x6E, "LD L, (HL)\0", 1, ld_l_at_hl},
	{0x6F, "LD L, A\0", 1, ld_l_a},

	{0x70, "LD (HL), B\0", 1, ld_at_hl_b},
	{0x71, "LD (HL), C\0", 1, ld_at_hl_c},
	{0x72, "LD (HL), D\0", 1, ld_at_hl_d},
	{0x73, "LD (HL), E\0", 1, ld_at_hl_e},
	{0x74, "LD (HL), H\0", 1, ld_at_hl_h},
	{0x75, "LD (HL), L\0", 1, ld_at_hl_l},
	{0x76, "HALT\0", 1, halt},
	{0x77, "LD (HL), A\0", 1,ld_at_hl_a},
	{0x78, "LD A, B\0", 1, ld_a_b},
	{0x79, "LD A, C\0", 1, ld_a_c},
	{0x7A, "LC A, D\0", 1, ld_a_d},
	{0x7B, "LD A, E\0", 1, ld_a_e},
	{0x7C, "LD A, H\0", 1, ld_a_h},
	{0x7D, "LD A, L\0", 1, ld_a_l},
	{0x7E, "LD A, (HL)\0", 1, ld_a_at_hl},
	{0x7F, "LD A, A\0", 1, ld_a_a},

	{0x80, "ADD A, B\0", 1, add_a_b},
	{0x81, "ADD A, C\0", 1, add_a_c},
	{0x82, "ADD A, D\0", 1, add_a_d},
	{0x83, "ADD A, E\0", 1, add_a_e},
	{0x84, "ADD A, H\0", 1, add_a_h},
	{0x85, "ADD A, L\0", 1, add_a_l},
	{0x86, "ADD A, (HL)\0", 1, add_a_at_hl},
	{0x87, "ADD A, A\0", 1, add_a_a},
	{0x88, "ADC A, B\0", 1, adc_a_b},
	{0x89, "ADC A, C\0", 1, adc_a_c},
	{0x8A, "ADC A, D\0", 1, adc_a_d},
	{0x8B, "ADC A, E\0", 1, adc_a_e},
	{0x8C, "ADC A, H\0", 1, adc_a_h},
	{0x8D, "ADC A, L\0", 1, adc_a_l},
	{0x8E, "ADC A, (HL)\0", 1, adc_a_at_hl},
	{0x8F, "ADC A, A\0", 1, adc_a_a},

	{0x90, "SUB B\0", 1, sub_b},
	{0x91, "SUB C\0", 1, sub_c},
	{0x92, "SUB D\0", 1, sub_d},
	{0x93, "SUB E\0", 1, sub_e},
	{0x94, "SUB H\0", 1, sub_h},
	{0x95, "SUB L\0", 1, sub_l},
	{0x96, "SUB (HL)\0", 1, sub_at_hl},
	{0x97, "SUB A\0", 1, sub_a},
	{0x98, "SBC A, B\0", 1, sbc_a_b},
	{0x99, "SBC A, C\0", 1, sbc_a_c},
	{0x9A, "SBC A, D\0", 1, sbc_a_d},
	{0x9B, "SBC A, E\0", 1, sbc_a_e},
	{0x9C, "SBC A, H\0", 1, sbc_a_h},
	{0x9D, "SBC A, L\0", 1, sbc_a_l},
	{0x9E, "SBC A, (HL)\0", 1, sbc_a_at_hl},
	{0x9F, "SBC A, A\0", 1, sbc_a_a},

	{0xA0, "AND B\0", 1, and_b},
	{0xA1, "AND C\0", 1, and_c},
	{0xA2, "AND D\0", 1, and_d},
	{0xA3, "AND E\0", 1, and_e},
	{0xA4, "AND H\0", 1, and_h},
	{0xA5, "AND L\0", 1, and_l},
	{0xA6, "AND (HL)\0", 1, and_at_hl},
	{0xA7, "AND A\0", 1, and_a},
	{0xA8, "XOR B\0", 1, xor_b},
	{0xA9, "XOR C\0", 1, xor_c},
	{0xAA, "XOR D\0", 1, xor_d},
	{0xAB, "XOR E\0", 1, xor_e},
	{0xAC, "XOR H\0", 1, xor_h},
	{0xAD, "XOR L\0", 1, xor_l},
	{0xAE, "XOR (HL)\0", 1, xor_at_hl},
	{0xAF, "XOR A\0", 1, xor_a},

	{0xB0, "OR B\0", 1, or_b},
	{0xB1, "OR C\0", 1, or_c},
	{0xB2, "OR D\0", 1, or_d},
	{0xB3, "OR E\0", 1, or_e},
	{0xB4, "OR H\0", 1, or_h},
	{0xB5, "OR L\0", 1, or_l},
	{0xB6, "OR (HL)\0", 1, or_at_hl},
	{0xB7, "OR A\0", 1, or_a},
	{0xB8, "CP B\0", 1, cp_b},
	{0xB9, "CP C\0", 1, cp_c},
	{0xBA, "CP D\0", 1, cp_d},
	{0xBB, "CP E\0", 1, cp_e},
	{0xBC, "CP H\0", 1, cp_h},
	{0xBD, "CP L\0", 1, cp_l},
	{0xBE, "CP (HL)\0", 1, cp_at_hl},
	{0xBF, "CP A\0", 1, cp_a},

	{0xC0, "RET NZ\0", 1, ret_nz},
	{0xC1, "POP BC\0", 1, pop_bc},
	{0xC2, "JP NZ, %04X\0", 3, jp_nz_a16},
	{0xC3, "JP %04X\0", 3, jp_a16},
	{0xC4, "CALL NZ, %04X\0", 3, call_nz_a16},
	{0xC5, "PUSH BC\0", 1, push_bc},
	{0xC6, "ADD A, %02X\0", 2, add_a_d8},
	{0xC7, "RST 00H\0", 1, rst_00h},
	{0xC8, "RET Z\0", 1, ret_z},
	{0xC9, "RET\0", 1, ret},
	{0xCA, "JP Z, %04X\0", 3, jp_z_a16},
	{0xCB, "INVALID\0", 0, NULL},
	{0xCC, "CALL Z, %04X\0", 3, call_z_a16},
	{0xCD, "CALL %04X\0", 3, call_a16},
	{0xCE, "ADC A, %02X\0", 2, adc_a_d8},
	{0xCF, "RST 08H\0", 1, rst_08h},

	{0xD0, "RET NC\0", 1, ret_nc},
	{0xD1, "POP DE\0", 1, pop_de},
	{0xD2, "JP NC, %04X\0", 3, jp_nc_a16},
	{0xD3, "INVALID\0", 1, NULL},
	{0xD4, "CALL NC, %04X\0", 3, call_nc_a16},
	{0xD5, "PUSH DE\0", 1, push_de},
	{0xD6, "SUB %02X\0", 2, sub_d8},
	{0xD7, "RST 10H\0", 1, rst_10h},
	{0xD8, "RET C\0", 1, ret_c},
	{0xD9, "RETI\0", 1, reti},
	{0xDA, "JP C, %04X\0", 3, jp_c_a16},
	{0xDB, "INVALID\0", 0, NULL},
	{0xDC, "CALL C, %04X\0", 3, call_c_a16},
	{0xDD, "INVALID\0", 0, NULL},
	{0xDE, "SBC A, %02X\0", 2, sbc_a_d8},
	{0xDF, "RST 18H\0", 1, rst_18h},

	{0xE0, "LDH, (%02X), A\0", 2, ldh_at_a8_a},
	{0xE1, "POP HL\0", 1, pop_hl},
	{0xE2, "LD (C), A\0", 1, ld_at_c_a},
	{0xE3, "INVAILD\0", 0, NULL},
	{0xE4, "INVALID\0", 0, NULL},
	{0xE5, "PUSH HL\0", 1, push_hl},
	{0xE6, "AND %02X\0", 2, and_d8},
	{0xE7, "RST 20H\0", 1, rst_20h},
	{0xE8, "ADD SP, %02X\0", 2, add_sp_r8},
	{0xE9, "JP HL\0", 1, jp_hl},
	{0xEA, "LD (%04X), A\0", 3, ld_at_a16_a},
	{0xEC, "INVALID\0", 0, NULL},
	{0xEB, "INVALID\0", 0, NULL},
	{0xED, "INVALID\0", 0, NULL},
	{0xEE, "XOR %02X\0", 2, xor_d8},
	{0xEF, "RST 28H\0", 1, rst_28h},

	{0xF0, "LDH A, (%02X)\0", 2, ldh_a_at_a8},
	{0xF1, "POP AF\0", 1, pop_af},
	{0xF2, "LD A, (C)\0", 1, ld_a_at_c},
	{0xF3, "DI\0", 1, di},
	{0xF4, "INVALID\0", 0, NULL},
	{0xF5, "PUSH AF\0", 1, push_af},
	{0xF6, "OR %02X\0", 2, or_d8},
	{0xF7, "RST 30H\0", 1, rst_30h},
	{0xF8, "LD HL, SP+%02X\0", 2, ld_hl_sp_plus_r8},
	{0xF9, "LD SP, HL\0", 1, ld_sp_hl},
	{0xFA, "LD A, (%04X)\0", 3, ld_a_at_a16},
	{0xFB, "EI\0", 1, ei},
	{0xFC, "INVALID\0", 0, NULL},
	{0xFD, "INVALID\0", 0, NULL},
	{0xFE, "CP %02X\0", 2, cp_d8},
	{0xFF, "RST 38H\0", 1, rst_38h}
};


//0x0x
void nop() {

}

void ld_bc_d16(uint16_t op16) {
	cpu.registers.bc = op16;
}

void ld_at_bc_a() {
	write_byte(cpu.registers.bc, cpu.registers.a);
}

void inc_bc() {
	cpu.registers.bc += 1;
}

void inc_b() {
	inc(&cpu.registers.b, &cpu.registers.f);
}

void dec_b() {
	dec(&cpu.registers.b, &cpu.registers.f);
}

void ld_b_d8(uint8_t op8) {
	cpu.registers.b = op8;
}

void rlca() {
	rlc(&cpu.registers.a, &cpu.registers.f);
	cpu.registers.f &= CF_MASK;
}

void ld_at_a16_sp(uint16_t op16) {
	write_byte(op16, cpu.registers.sp_low);
	write_byte(op16 + 1, cpu.registers.sp_high);
}

void add_hl_bc() {
	add16(&cpu.registers.hl, cpu.registers.bc, &cpu.registers.f);
}

void ld_a_at_bc() {
	cpu.registers.a = read_byte(cpu.registers.bc);
}

void dec_bc() {
	cpu.registers.bc -= 1;
}

void inc_c() {
	inc(&cpu.registers.c, &cpu.registers.f);
}

void dec_c() {
	dec(&cpu.registers.c, &cpu.registers.f);
}

void ld_c_d8(uint8_t op8) {
	cpu.registers.c = op8;
}

void rrca() {
	rrc(&cpu.registers.a, &cpu.registers.f);
	cpu.registers.f &= CF_MASK;
}


//0x1x
void stop() {
	cpu.stop = true;
}

void ld_de_d16(uint16_t op16) {
	cpu.registers.de = op16;
}

void ld_at_de_a() {
	write_byte(cpu.registers.de, cpu.registers.a);
}

void inc_de() {
	cpu.registers.de += 1;
}

void inc_d() {
	inc(&cpu.registers.d, &cpu.registers.f);
}

void dec_d() {
	dec(&cpu.registers.d, &cpu.registers.f);
}

void ld_d_d8(uint8_t op8) {
	cpu.registers.d = op8;
}

void rla() {
	rl(&cpu.registers.a, &cpu.registers.f);
	cpu.registers.f &= CF_MASK;
}

void jr_r8(uint8_t op8) {
	//2S COMPLIMENT ONLY
	if (op8 & 128) {
		//Negative
		op8 = (~op8) + 1;
		cpu.registers.pc -= op8;
	} 
	else {
		cpu.registers.pc += op8;
	}
}

void add_hl_de() {
	add16(&cpu.registers.hl, cpu.registers.de, &cpu.registers.f);
}

void ld_a_at_de() {
	cpu.registers.a = read_byte(cpu.registers.de);
}

void dec_de() {
	cpu.registers.de -= 1;
}

void inc_e() {
	inc(&cpu.registers.e, &cpu.registers.f);
}

void dec_e() {
	dec(&cpu.registers.e, &cpu.registers.f);
}

void ld_e_d8(uint8_t op8) {
	cpu.registers.e = op8;
}

void rra() {
	rr(&cpu.registers.a, &cpu.registers.f);
	cpu.registers.f &= CF_MASK;
}


//0x2x
void jr_nz_r8(uint8_t op8) {
	//opcode 0x20
	//Jump if zf is clear
	if (!(cpu.registers.f & ZF_MASK)) {
		jr_r8(op8);
		instruction_cycles[0x20] = instruction_cycles_when_conditionals_taken[0x20];
	}
	else {
		instruction_cycles[0x20] = instruction_cycles_when_conditionals_not_taken[0x20];
	}
}

void ld_hl_d16(uint16_t op16) {
	cpu.registers.hl = op16;
}

void ldi_at_hl_a() {
	//LD (HL+), A
	write_byte(cpu.registers.hl++, cpu.registers.a);
}

void inc_hl() {
	cpu.registers.hl += 1;
}

void inc_h() {
	inc(&cpu.registers.h, &cpu.registers.f);
}

void dec_h() {
	dec(&cpu.registers.h, &cpu.registers.f);
}

void ld_h_d8(uint8_t op8) {
	cpu.registers.h = op8;
}

void daa() {

	//Using blargg's daa code from
	//http://forums.nesdev.com/viewtopic.php?t=4728&start=15
	uint16_t a = cpu.registers.a;
	if (!(cpu.registers.f & 0x40)) {
		//Last arithmetic operation was not subtraction
		if ((cpu.registers.f & 0x20) || (a & 0x0F) > 9) {
			a += 0x06;
		}
		if ((cpu.registers.f & 0x10) || a > 0x9F) {
			a += 0x60;
		}
	}
	else {
		if(cpu.registers.f & 0x20) {
			a = (a - 6) & 0xFF;
		}
		if (cpu.registers.f & 0x10) {
			a -= 0x60;
		}
	}

	cpu.registers.f &= ~(0x20 | 0x80);
	if (a & 0x0100)
		cpu.registers.f |= CF_MASK;
	a &= 0xFF;
	if (!a)
		cpu.registers.f |= ZF_MASK;
	cpu.registers.a = (uint8_t)a;
}

void jr_z_r8(uint8_t op8) {
	//opcode 0x28
	//Jump if zf is set
	if (cpu.registers.f & ZF_MASK) {
		jr_r8(op8);
		instruction_cycles[0x28] = instruction_cycles_when_conditionals_taken[0x28];
	}
	else {
		instruction_cycles[0x28] = instruction_cycles_when_conditionals_not_taken[0x28];
	}
}

void add_hl_hl() {
	add16(&cpu.registers.hl, cpu.registers.hl, &cpu.registers.f);
}

void ldi_a_at_hl() {
	//LD A, (HL+)
	cpu.registers.a = read_byte(cpu.registers.hl++);
}

void dec_hl() {
	cpu.registers.hl -= 1;
}

void inc_l() {
	inc(&cpu.registers.l, &cpu.registers.f);
}

void dec_l() {
	dec(&cpu.registers.l, &cpu.registers.f);
}

void ld_l_d8(uint8_t op8) {
	cpu.registers.l = op8;
}

void cpl() {
	//Inverts A
	//Flags:  - 1 1 -
	cpu.registers.a = ~cpu.registers.a;
	cpu.registers.f |= SF_MASK;
	cpu.registers.f |= HC_MASK;
}

//$3x
void jr_nc_r8(uint8_t op8) {
	//opcode 0x30
	//Jump if cf is clear
	if (!(cpu.registers.f & CF_MASK)) {
		jr_r8(op8);
		instruction_cycles[0x30] = instruction_cycles_when_conditionals_taken[0x30];
	}
	else {
		instruction_cycles[0x30] = instruction_cycles_when_conditionals_not_taken[0x30];
	}
}

void ld_sp_d16(uint16_t op16) {
	cpu.registers.sp = op16;
}

void ldd_at_hl_a() {
	//LD (HL-), A
	write_byte(cpu.registers.hl--, cpu.registers.a);
}

void inc_sp() {
	cpu.registers.sp += 1;
}

void inc_at_hl() {
	uint8_t r8 = read_byte(cpu.registers.hl);
	inc(&r8, &cpu.registers.f);
	write_byte(cpu.registers.hl, r8);
}

void dec_at_hl() {
	uint8_t r8 = read_byte(cpu.registers.hl);
	dec(&r8, &cpu.registers.f);
	write_byte(cpu.registers.hl, r8);
}

void ld_at_hl_d8(uint8_t op8) {
	//LD (HL), d8
	write_byte(cpu.registers.hl, op8);
}

void scf() {
	//Flags  - 0 0 1
	//Sets carry flag
	cpu.registers.f &= ZF_MASK;
	cpu.registers.f |= CF_MASK;
}

void jr_c_r8(uint8_t op8) {
	//opcode 0x38
	//Jump if carry is set
	if (cpu.registers.f & CF_MASK) {
		jr_r8(op8);
		instruction_cycles[0x38] = instruction_cycles_when_conditionals_taken[0x38];
	}
	else {
		instruction_cycles[0x38] = instruction_cycles_when_conditionals_not_taken[0x38];
	}
}

void add_hl_sp() {
	add16(&cpu.registers.hl, cpu.registers.sp, &cpu.registers.f);
}

void ldd_a_at_hl() {
	//LD A, (HL-)
	cpu.registers.a = read_byte(cpu.registers.hl--);
}

void dec_sp() {
	cpu.registers.sp -= 1;
}

void inc_a() {
	inc(&cpu.registers.a, &cpu.registers.f);
}

void dec_a() {
	dec(&cpu.registers.a, &cpu.registers.f);
}

void ld_a_d8(uint8_t op8) {
	cpu.registers.a = op8;
}

void ccf() {
	//Flags: - 0 0 C
	//Inverts carry flag
	cpu.registers.f = (cpu.registers.f & ZF_MASK) | ((~cpu.registers.f) & CF_MASK);
}


//$4x
void ld_b_b() {
	//do nothing
}

void ld_b_c() {
	cpu.registers.b = cpu.registers.c;
}

void ld_b_d() {
	cpu.registers.b = cpu.registers.d;
}

void ld_b_e() {
	cpu.registers.b = cpu.registers.e;
}

void ld_b_f() {
	cpu.registers.b = cpu.registers.f;
}

void ld_b_h() {
	cpu.registers.b = cpu.registers.h;
}

void ld_b_l() {
	cpu.registers.b = cpu.registers.l;
}

void ld_b_at_hl() {
	cpu.registers.b = read_byte(cpu.registers.hl);
}

void ld_b_a() {
	cpu.registers.b = cpu.registers.a;
}

void ld_c_b() {
	cpu.registers.c = cpu.registers.b;
}

void ld_c_c() {
	cpu.registers.c = cpu.registers.c;
}

void ld_c_d() {
	cpu.registers.c = cpu.registers.d;
}

void ld_c_e() {
	cpu.registers.c = cpu.registers.e;
}

void ld_c_h() {
	cpu.registers.c = cpu.registers.h;
}

void ld_c_l() {
	cpu.registers.c = cpu.registers.l;
}

void ld_c_at_hl() {
	cpu.registers.c = read_byte(cpu.registers.hl);
}

void ld_c_a() {
	cpu.registers.c = cpu.registers.a;
}

void ld_d_b() {
	cpu.registers.d = cpu.registers.b;
}

void ld_d_c() {
	cpu.registers.d = cpu.registers.c;
}

void ld_d_d() {
	cpu.registers.d = cpu.registers.d;
}

void ld_d_e() {
	cpu.registers.d = cpu.registers.e;
}

void ld_d_f() {
	cpu.registers.d = cpu.registers.f;
}

void ld_d_h() {
	cpu.registers.d = cpu.registers.h;
}

void ld_d_l() {
	cpu.registers.d = cpu.registers.l;
}

void ld_d_at_hl() {
	cpu.registers.d = read_byte(cpu.registers.hl);
}

void ld_d_a() {
	cpu.registers.d = cpu.registers.a;
}

void ld_e_b() {
	cpu.registers.e = cpu.registers.b;
}

void ld_e_c() {
	cpu.registers.e = cpu.registers.c;
}

void ld_e_d() {
	cpu.registers.e = cpu.registers.d;
}

void ld_e_e() {
	cpu.registers.e = cpu.registers.e;
}

void ld_e_h() {
	cpu.registers.e = cpu.registers.h;
}

void ld_e_l() {
	cpu.registers.e = cpu.registers.l;
}

void ld_e_at_hl() {
	cpu.registers.e = read_byte(cpu.registers.hl);
}

void ld_e_a() {
	cpu.registers.e = cpu.registers.a;
}

//$6x
void ld_h_b() {
	cpu.registers.h = cpu.registers.b;
}

void ld_h_c() {
	cpu.registers.h = cpu.registers.c;
}

void ld_h_d() {
	cpu.registers.h = cpu.registers.d;
}

void ld_h_e() {
	cpu.registers.h = cpu.registers.e;
}

void ld_h_f() {
	cpu.registers.h = cpu.registers.f;
}

void ld_h_h() {
	cpu.registers.h = cpu.registers.h;
}

void ld_h_l() {
	cpu.registers.h = cpu.registers.l;
}

void ld_h_at_hl() {
	cpu.registers.h = read_byte(cpu.registers.hl);
}

void ld_h_a() {
	cpu.registers.h = cpu.registers.a;
}

void ld_l_b() {
	cpu.registers.l = cpu.registers.b;
}

void ld_l_c() {
	cpu.registers.l = cpu.registers.c;
}

void ld_l_d() {
	cpu.registers.l = cpu.registers.d;
}

void ld_l_e() {
	cpu.registers.l = cpu.registers.e;
}

void ld_l_h() {
	cpu.registers.l = cpu.registers.h;
}

void ld_l_l() {
	cpu.registers.l = cpu.registers.l;
}

void ld_l_at_hl() {
	cpu.registers.l = read_byte(cpu.registers.hl);
}

void ld_l_a() {
	cpu.registers.l = cpu.registers.a;
}

//$7x
void ld_at_hl_b() {
	write_byte(cpu.registers.hl, cpu.registers.b);
}

void ld_at_hl_c() {
	write_byte(cpu.registers.hl, cpu.registers.c);
}

void ld_at_hl_d() {
	write_byte(cpu.registers.hl, cpu.registers.d);
}

void ld_at_hl_e() {
	write_byte(cpu.registers.hl, cpu.registers.e);
}

void halt() {
	cpu.halt = true;
}

void ld_at_hl_h() {
	write_byte(cpu.registers.hl, cpu.registers.h);
}

void ld_at_hl_l() {
	write_byte(cpu.registers.hl, cpu.registers.l);
}

void ld_at_hl_a() {
	write_byte(cpu.registers.hl, cpu.registers.a);
}

void ld_a_b() {
	cpu.registers.a = cpu.registers.b;
}

void ld_a_c() {
	cpu.registers.a = cpu.registers.c;
}

void ld_a_d() {
	cpu.registers.a = cpu.registers.d;
}

void ld_a_e() {
	cpu.registers.a = cpu.registers.e;
}

void ld_a_h() {
	cpu.registers.a = cpu.registers.h;
}

void ld_a_l() {
	cpu.registers.a = cpu.registers.l;
}

void ld_a_at_hl() {
	cpu.registers.a = read_byte(cpu.registers.hl);
}

void ld_a_a() {
	cpu.registers.a = cpu.registers.a;
}

//$8x
void add_a_b() {
	add8(&cpu.registers.a, cpu.registers.b, &cpu.registers.f);
}

void add_a_c() {
	add8(&cpu.registers.a, cpu.registers.c, &cpu.registers.f);
}

void add_a_d() {
	add8(&cpu.registers.a, cpu.registers.d, &cpu.registers.f);
}

void add_a_e() {
	add8(&cpu.registers.a, cpu.registers.e, &cpu.registers.f);
}

void add_a_h() {
	add8(&cpu.registers.a, cpu.registers.h, &cpu.registers.f);
}

void add_a_l() {
	add8(&cpu.registers.a, cpu.registers.l, &cpu.registers.f);
}
void add_a_at_hl() {
	add8(&cpu.registers.a, read_byte(cpu.registers.hl), &cpu.registers.f);
}

void add_a_a() {
	add8(&cpu.registers.a, cpu.registers.a, &cpu.registers.f);
}

void adc_a_b() {
	adc(&cpu.registers.a, cpu.registers.b, &cpu.registers.f);
}

void adc_a_c() {
	adc(&cpu.registers.a, cpu.registers.c, &cpu.registers.f);
}

void adc_a_d() {
	adc(&cpu.registers.a, cpu.registers.d, &cpu.registers.f);
}

void adc_a_e() {
	adc(&cpu.registers.a, cpu.registers.e, &cpu.registers.f);
}

void adc_a_h() {
	adc(&cpu.registers.a, cpu.registers.h, &cpu.registers.f);
}

void adc_a_l() {
	adc(&cpu.registers.a, cpu.registers.l, &cpu.registers.f);
}

void adc_a_at_hl() {
	adc(&cpu.registers.a, read_byte(cpu.registers.hl), &cpu.registers.f);
}

void adc_a_a() {
	adc(&cpu.registers.a, cpu.registers.a, &cpu.registers.f);
}


//$9x
void sub_b() {
	sub(&cpu.registers.a, cpu.registers.b, &cpu.registers.f);
}

void sub_c() {
	sub(&cpu.registers.a, cpu.registers.c, &cpu.registers.f);
}

void sub_d() {
	sub(&cpu.registers.a, cpu.registers.d, &cpu.registers.f);
}

void sub_e() {
	sub(&cpu.registers.a, cpu.registers.e, &cpu.registers.f);
}

void sub_h() {
	sub(&cpu.registers.a, cpu.registers.h, &cpu.registers.f);
}

void sub_l() {
	sub(&cpu.registers.a, cpu.registers.l, &cpu.registers.f);
}

void sub_at_hl() {
	sub(&cpu.registers.a, read_byte(cpu.registers.hl), &cpu.registers.f);
}

void sub_a() {
	sub(&cpu.registers.a, cpu.registers.a, &cpu.registers.f);
}

void sbc_a_b() {
	sbc(&cpu.registers.a, cpu.registers.b, &cpu.registers.f);
}

void sbc_a_c() {
	sbc(&cpu.registers.a, cpu.registers.c, &cpu.registers.f);
}

void sbc_a_d() {
	sbc(&cpu.registers.a, cpu.registers.d, &cpu.registers.f);
}

void sbc_a_e() {
	sbc(&cpu.registers.a, cpu.registers.e, &cpu.registers.f);
}

void sbc_a_h() {
	sbc(&cpu.registers.a, cpu.registers.h, &cpu.registers.f);
}

void sbc_a_l() {
	sbc(&cpu.registers.a, cpu.registers.l, &cpu.registers.f);
}

void sbc_a_at_hl() {
	sbc(&cpu.registers.a, read_byte(cpu.registers.hl), &cpu.registers.f);
}

void sbc_a_a() {
	sbc(&cpu.registers.a, cpu.registers.a, &cpu.registers.f);
}

//$Ax
void and_b() {
	and (&cpu.registers.a, cpu.registers.b, &cpu.registers.f);
}

void and_c() {
	and (&cpu.registers.a, cpu.registers.c, &cpu.registers.f);
}

void and_d() {
	and (&cpu.registers.a, cpu.registers.d, &cpu.registers.f);
}

void and_e() {
	and (&cpu.registers.a, cpu.registers.e, &cpu.registers.f);
}

void and_h() {
	and (&cpu.registers.a, cpu.registers.h, &cpu.registers.f);
}

void and_l() {
	and (&cpu.registers.a, cpu.registers.l, &cpu.registers.f);
}

void and_at_hl() {
	and (&cpu.registers.a, read_byte(cpu.registers.hl), &cpu.registers.f);
}

void and_a() {
	and (&cpu.registers.a, cpu.registers.a, &cpu.registers.f);
}

void xor_b() {
	xor (&cpu.registers.a, cpu.registers.b, &cpu.registers.f);
}

void xor_c() {
	xor (&cpu.registers.a, cpu.registers.c, &cpu.registers.f);
}

void xor_d() {
	xor (&cpu.registers.a, cpu.registers.d, &cpu.registers.f);
}

void xor_e() {
	xor (&cpu.registers.a, cpu.registers.e, &cpu.registers.f);
}

void xor_h() {
	xor (&cpu.registers.a, cpu.registers.h, &cpu.registers.f);
}

void xor_l() {
	xor (&cpu.registers.a, cpu.registers.l, &cpu.registers.f);
}

void xor_at_hl() {
	xor (&cpu.registers.a, read_byte(cpu.registers.hl), &cpu.registers.f);
}

void xor_a() {
	xor (&cpu.registers.a, cpu.registers.a, &cpu.registers.f);
}


//$Bx
void or_b() {
	or (&cpu.registers.a, cpu.registers.b, &cpu.registers.f);
}

void or_c() {
	or (&cpu.registers.a, cpu.registers.c, &cpu.registers.f);
}

void or_d() {
	or (&cpu.registers.a, cpu.registers.d, &cpu.registers.f);
}

void or_e() {
	or (&cpu.registers.a, cpu.registers.e, &cpu.registers.f);
}

void or_h() {
	or (&cpu.registers.a, cpu.registers.h, &cpu.registers.f);
}

void or_l() {
	or (&cpu.registers.a, cpu.registers.l, &cpu.registers.f);
}

void or_at_hl() {
	or (&cpu.registers.a, read_byte(cpu.registers.hl), &cpu.registers.f);
}

void or_a() {
	or (&cpu.registers.a, cpu.registers.a, &cpu.registers.f);
}

void cp_b() {
	cp (cpu.registers.a, cpu.registers.b, &cpu.registers.f);
}

void cp_c() {
	cp (cpu.registers.a, cpu.registers.c, &cpu.registers.f);
}

void cp_d() {
	cp (cpu.registers.a, cpu.registers.d, &cpu.registers.f);
}

void cp_e() {
	cp (cpu.registers.a, cpu.registers.e, &cpu.registers.f);
}

void cp_h() {
	cp (cpu.registers.a, cpu.registers.h, &cpu.registers.f);
}

void cp_l() {
	cp (cpu.registers.a, cpu.registers.l, &cpu.registers.f);
}

void cp_at_hl() {
	cp (cpu.registers.a, read_byte(cpu.registers.hl), &cpu.registers.f);
}

void cp_a() {
	cp (cpu.registers.a, cpu.registers.a, &cpu.registers.f);
}

//$Cx
void ret_nz() {
	//opcode 0xC0
	//ret if zf is clear
	if (!(cpu.registers.f & ZF_MASK)) {
		ret();
		instruction_cycles[0xC0] = instruction_cycles_when_conditionals_taken[0xC0];
	}
	else {
		instruction_cycles[0xC0] = instruction_cycles_when_conditionals_not_taken[0xC0];
	}
}

void pop_bc() {
	cpu.registers.bc = pop();
}

void jp_nz_a16(uint16_t op16) {
	//opcode 0xC2
	if (!(cpu.registers.f & ZF_MASK)) {
		jp_a16(op16);
		instruction_cycles[0xC2] = instruction_cycles_when_conditionals_taken[0xC2];
	}
	else {
		instruction_cycles[0xC2] = instruction_cycles_when_conditionals_not_taken[0xC2];
	}
}

void jp_a16(uint16_t op16) {
	cpu.registers.pc = op16;
}

void call_nz_a16(uint16_t op16) {
	//opcode 0xC4
	if (!(cpu.registers.f & ZF_MASK)) {
		call_a16(op16);
		instruction_cycles[0xC4] = instruction_cycles_when_conditionals_taken[0xC4];
	}
	else {
		instruction_cycles[0xC4] = instruction_cycles_when_conditionals_not_taken[0xC4];
	}
}

void push_bc() {
	push(cpu.registers.bc);
}

void add_a_d8(uint8_t op8) {
	add8(&cpu.registers.a, op8, &cpu.registers.f);
}

void rst_00h() {
	call_a16(0);
}

void ret_z() {
	//opcode 0xC8
	if (cpu.registers.f & ZF_MASK) {
		ret();
		instruction_cycles[0xC8] = instruction_cycles_when_conditionals_taken[0xC8];
	}
	else {
		instruction_cycles[0xC8] = instruction_cycles_when_conditionals_not_taken[0xC8];
	}
}

void ret() {
	cpu.registers.pc = pop();
}

void jp_z_a16(uint16_t op16) {
	//opcode 0xCA
	if (cpu.registers.f & ZF_MASK) {
		jp_a16(op16);
		instruction_cycles[0xCA] = instruction_cycles_when_conditionals_taken[0xCA];
	}
	else {
		instruction_cycles[0xCA] = instruction_cycles_when_conditionals_not_taken[0xCA];
	}
}

//PREFIX CB: INVALID

void call_z_a16(uint16_t op16) {
	//opcode 0xCC
	if(cpu.registers.f & ZF_MASK) {
		call_a16(op16);
		instruction_cycles[0xCC] = instruction_cycles_when_conditionals_taken[0xCC];
	}
	else {
		instruction_cycles[0xCC] = instruction_cycles_when_conditionals_not_taken[0xCC];
	}
}

void call_a16(uint16_t op16) {
	push(cpu.registers.pc);
	cpu.registers.pc = op16;
}

void adc_a_d8(uint8_t op8) {
	adc(&cpu.registers.a, op8, &cpu.registers.f);
}

void rst_08h() {
	call_a16(0x08);
}

//$Dx
void ret_nc() {
	//opcode 0xD0
	if (!(cpu.registers.f & CF_MASK)) {
		ret();
		instruction_cycles[0xD0] = instruction_cycles_when_conditionals_taken[0xD0];
	}
	else {
		instruction_cycles[0xD0] = instruction_cycles_when_conditionals_not_taken[0xD0];
	}
}

void pop_de() {
	cpu.registers.de = pop();
}

void jp_nc_a16(uint16_t op16) {
	//opcode 0xD2
	if (!(cpu.registers.f & CF_MASK)) {
		jp_a16(op16);
		instruction_cycles[0xD2] = instruction_cycles_when_conditionals_taken[0xD2];
	}
	else {
		instruction_cycles[0xD2] = instruction_cycles_when_conditionals_not_taken[0xD2];
	}
}

//0xD3: INVALID

void call_nc_a16(uint16_t op16) {
	//opcode 0xD4
	if (!(cpu.registers.f & CF_MASK)) {
		call_a16(op16);
		instruction_cycles[0xD4] = instruction_cycles_when_conditionals_taken[0xD4];
	}
	else {
		instruction_cycles[0xD4] = instruction_cycles_when_conditionals_not_taken[0xD4];
	}
}

void push_de() {
	push(cpu.registers.de);
}

void sub_d8(uint8_t op8) {
	sub(&cpu.registers.a, op8, &cpu.registers.f);
}

void rst_10h() {
	call_a16(0x10);
}

void ret_c() {
	//opcode 0xD8
	if (cpu.registers.f & CF_MASK) {
		ret();
		instruction_cycles[0xD8] = instruction_cycles_when_conditionals_taken[0xD8];
	}
	else {
		instruction_cycles[0xD8] = instruction_cycles_when_conditionals_not_taken[0xD8];
	}
}

void reti() {
	ret();
	ei();
}

void jp_c_a16(uint16_t op16) {
	//opcode 0xDA
	if (cpu.registers.f & CF_MASK) {
		jp_a16(op16);
		instruction_cycles[0xDA] = instruction_cycles_when_conditionals_taken[0xDA];
	}
	else {
		instruction_cycles[0xDA] = instruction_cycles_when_conditionals_taken[0xDA];
	}
}

//0xDB: INVALID

void call_c_a16(uint16_t op16) {
	//opcode 0xDC
	if(cpu.registers.f & CF_MASK) {
		call_a16(op16);
		instruction_cycles[0xDC] = instruction_cycles_when_conditionals_taken[0xDC];
	}
	else {
		instruction_cycles[0xDC] = instruction_cycles_when_conditionals_not_taken[0xDC];
	}
}

//0xDD: INVALID
void sbc_a_d8(uint8_t op8) {
	sbc(&cpu.registers.a, op8, &cpu.registers.f);
}

void rst_18h() {
	call_a16(0x18);
}

//$Ex
void ldh_at_a8_a(uint8_t op8) {
	//TODO: Add bounds checking?
	write_byte(0xFF00 + op8, cpu.registers.a);
}

void pop_hl() {
	cpu.registers.hl = pop();
}

void ld_at_c_a() {
	//TODO: Add bounds checking?
	write_byte(0xFF00 + cpu.registers.c, cpu.registers.a);
}

//0xE3: INVALID

//0xE4: INVALID

void push_hl() {
	push(cpu.registers.hl);
}

void and_d8(uint8_t op8) {
	and (&cpu.registers.a, op8, &cpu.registers.f);
}

void rst_20h() {
	call_a16(0x20);
}

void add_sp_r8(uint8_t op8) {
	//Flags: 0 0 H C
	//Note: according to pan docs, op8 is a signed number
	add_sp_nn(&cpu.registers.sp, op8, &cpu.registers.f);
}

void jp_hl() {
	//according to bgb this jumps to the address in register hl instead of reading the byte at hl
	//cpu.registers.pc = read_byte(cpu.registers.hl);
	cpu.registers.pc = cpu.registers.hl;
}

void ld_at_a16_a(uint16_t op16) {
	write_byte(op16, cpu.registers.a);
}

//0xEB: INVALID

//0xEC: INVALID

//0xED: INVALID

void xor_d8(uint8_t op8) {
	xor (&cpu.registers.a, op8, &cpu.registers.f);
}

void rst_28h() {
	call_a16(0x28);
}


//$Fx
void ldh_a_at_a8(uint8_t op8) {
	//TODO: Add bounds check.
	cpu.registers.a = memory.io[op8];
}

void pop_af() {
	cpu.registers.af = pop();
	cpu.registers.f &= 0b11110000;	//clear low bits of f
}

void ld_a_at_c() {
	//TODO: Add bounds check.
	cpu.registers.a = memory.io[cpu.registers.c];
}

void di() {
	cpu.ime = false;
}

//0xF4: INVALID

void push_af() {
	push(cpu.registers.af);
}

void or_d8(uint8_t op8) {
	or (&cpu.registers.a, op8, &cpu.registers.f);
}

void rst_30h() {
	call_a16(0x18);
}

void ld_hl_sp_plus_r8(uint8_t op8) {
	//Flags: 0 0 H C
	cpu.registers.hl = cpu.registers.sp;
	add_sp_nn(&cpu.registers.hl, op8, &cpu.registers.f);
}

void ld_sp_hl() {
	cpu.registers.sp = cpu.registers.hl;
}

void ld_a_at_a16(uint16_t op16) {
	cpu.registers.a = read_byte(op16);
}

void ei() {
	cpu.ime = true;
}

//0xFC: INVALID

//0xFD: INVALID

void cp_d8(uint8_t op8) {
	cp(cpu.registers.a, op8, &cpu.registers.f);
}

void rst_38h() {
	call_a16(0x38);
}