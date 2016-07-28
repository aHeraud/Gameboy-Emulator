#include<stdint.h>
#include"instructions.h"
#include"cpu.h"
#include"mmu.h"
#include"register8.h"

//Instruction cycle tables from the readme of blargg's instr_timing test rom
uint8_t extended_instruction_cycles[256] = {
		2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2,
		2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2,
		2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2,
		2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2,
		2,2,2,2,2,2,3,2,2,2,2,2,2,2,3,2,
		2,2,2,2,2,2,3,2,2,2,2,2,2,2,3,2,
		2,2,2,2,2,2,3,2,2,2,2,2,2,2,3,2,
		2,2,2,2,2,2,3,2,2,2,2,2,2,2,3,2,
		2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2,
		2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2,
		2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2,
		2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2,
		2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2,
		2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2,
		2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2,
		2,2,2,2,2,2,4,2,2,2,2,2,2,2,4,2
};

const instruction_t extended_instructions[256] = {
	{ 0x00, "RLC B\0", 2, rlc_b },
	{ 0x01, "RLC C\0", 2, rlc_c },
	{ 0x02, "RLC D\0", 2, rlc_d },
	{ 0x03, "RLC E\0", 2, rlc_e },
	{ 0x04, "RLC H\0", 2, rlc_h },
	{ 0x05, "RLC L\0", 2, rlc_l },
	{ 0x06, "RLC (HL)\0", 2, rlc_at_hl },
	{ 0x07, "RLC A\0", 2, rlc_a },
	{ 0x08, "RRC B\0", 2, rrc_b },
	{ 0x09, "RRC C\0", 2, rrc_c },
	{ 0x0A, "RRC D\0", 2, rrc_d },
	{ 0x0B, "RRC E\0", 2, rrc_e },
	{ 0x0C, "RRC H\0", 2, rrc_h },
	{ 0x0D, "RRC L\0", 2, rrc_l },
	{ 0x0E, "RRC (HL)\0", 2, rrc_at_hl },
	{ 0x0F, "RRC A\0", 2, rrc_a },

	{ 0x10, "RL B\0", 2, rl_b },
	{ 0x11, "RL C\0", 2, rl_c },
	{ 0x12, "RL D\0", 2, rl_d },
	{ 0x13, "RL E\0", 2, rl_e },
	{ 0x14, "RL H\0", 2, rl_h },
	{ 0x15, "RL L\0", 2, rl_l },
	{ 0x16, "RL (HL)\0", 2, rl_at_hl },
	{ 0x17, "RL A\0", 2, rl_a },
	{ 0x18, "RR B\0", 2, rr_b },
	{ 0x19, "RR C\0", 2, rr_c },
	{ 0x1A, "RR D\0", 2, rr_d },
	{ 0x1B, "RR E\0", 2, rr_e },
	{ 0x1C, "RR H\0", 2, rr_h },
	{ 0x1D, "RR L\0", 2, rr_l },
	{ 0x1E, "RR (HL)\0", 2, rr_at_hl },
	{ 0x1F, "RR A\0", 2, rr_a },

	{ 0x20, "SLA B\0", 2, sla_b },
	{ 0x21, "SLA C\0", 2, sla_c },
	{ 0x22, "SLA D\0", 2, sla_d },
	{ 0x23, "SLA E\0", 2, sla_e },
	{ 0x24, "SLA H\0", 2, sla_h },
	{ 0x25, "SLA L\0", 2, sla_l },
	{ 0x26, "SLA (HL)\0", 2, sla_at_hl },
	{ 0x27, "SLA A\0", 2, sla_a },
	{ 0x28, "SRA B\0", 2, sra_b },
	{ 0x29, "SRA C\0", 2, sra_c },
	{ 0x2A, "SRA D\0", 2, sra_d },
	{ 0x2B, "SRA E\0", 2, sra_e },
	{ 0x2C, "SRA H\0", 2, sra_h },
	{ 0x2D, "SRA L\0", 2, sra_l },
	{ 0x2E, "SRA (HL)\0", 2, sra_at_hl },
	{ 0x2F, "SRA A\0", 2, sra_a },

	{ 0x30, "SWAP B\0", 2, swap_b },
	{ 0x31, "SWAP C\0", 2, swap_c },
	{ 0x32, "SWAP D\0", 2, swap_d },
	{ 0x33, "SWAP E\0", 2, swap_e },
	{ 0x34, "SWAP H\0", 2, swap_h },
	{ 0x35, "SWAP L\0", 2, swap_l },
	{ 0x36, "SWAP (HL)\0", 2, swap_at_hl },
	{ 0x37, "SWAP A\0", 2, swap_a },
	{ 0x38, "SRL B\0", 2, srl_b },
	{ 0x39, "SRL C\0", 2, srl_c },
	{ 0x3A, "SRL D\0", 2, srl_d },
	{ 0x3B, "SRL E\0", 2, srl_e },
	{ 0x3C, "SRL H\0", 2, srl_h },
	{ 0x3D, "SRL L\0", 2, srl_l },
	{ 0x3E, "SRL (HL)\0", 2, srl_at_hl },
	{ 0x3F, "SRL A\0", 2, srl_a },

	{ 0x40, "BIT 0, B\0", 2, bit_0_b },
	{ 0x41, "BIT 0, C\0", 2, bit_0_c },
	{ 0x42, "BIT 0, D\0", 2, bit_0_d },
	{ 0x43, "BIT 0, E\0", 2, bit_0_e },
	{ 0x44, "BIT 0, H\0", 2, bit_0_h },
	{ 0x45, "BIT 0, L\0", 2, bit_0_l },
	{ 0x46, "BIT 0, (HL)\0", 2, bit_0_at_hl },
	{ 0x47, "BIT 0, A\0", 2, bit_0_a },
	{ 0x48, "BIT 1, B\0", 2, bit_1_b },
	{ 0x49, "BIT 1, C\0", 2, bit_1_c },
	{ 0x4A, "BIT 1, D\0", 2, bit_1_d },
	{ 0x4B, "BIT 1, E\0", 2, bit_1_e },
	{ 0x4C, "BIT 1, H\0", 2, bit_1_h },
	{ 0x4D, "BIT 1, L\0", 2, bit_1_l },
	{ 0x4E, "BIT 1, (HL)\0", 2, bit_1_at_hl },
	{ 0x4F, "BIT 1, A\0", 2, bit_1_a },

	{ 0x50, "BIT 2, B\0", 2, bit_2_b },
	{ 0x51, "BIT 2, C\0", 2, bit_2_c },
	{ 0x52, "BIT 2, D\0", 2, bit_2_d },
	{ 0x53, "BIT 2, E\0", 2, bit_2_e },
	{ 0x54, "BIT 2, H\0", 2, bit_2_h },
	{ 0x55, "BIT 2, L\0", 2, bit_2_l },
	{ 0x56, "BIT 2, (HL)\0", 2, bit_2_at_hl },
	{ 0x57, "BIT 2, A\0", 2, bit_2_a },
	{ 0x58, "BIT 3, B\0", 2, bit_3_b },
	{ 0x59, "BIT 3, C\0", 2, bit_3_c },
	{ 0x5A, "BIT 3, D\0", 2, bit_3_d },
	{ 0x5B, "BIT 3, E\0", 2, bit_3_e },
	{ 0x5C, "BIT 3, H\0", 2, bit_3_h },
	{ 0x5D, "BIT 3, L\0", 2, bit_3_l },
	{ 0x5E, "BIT 3, (HL)\0", 2, bit_3_at_hl },
	{ 0x5F, "BIT 3, A\0", 2, bit_3_a },

	{ 0x60, "BIT 4, B\0", 2, bit_4_b },
	{ 0x61, "BIT 4, C\0", 2, bit_4_c },
	{ 0x62, "BIT 4, D\0", 2, bit_4_d },
	{ 0x63, "BIT 4, E\0", 2, bit_4_e },
	{ 0x64, "BIT 4, H\0", 2, bit_4_h },
	{ 0x65, "BIT 4, L\0", 2, bit_4_l },
	{ 0x66, "BIT 4, (HL)\0", 2, bit_4_at_hl },
	{ 0x67, "BIT 4, A\0", 2, bit_4_a },
	{ 0x68, "BIT 5, B\0", 2, bit_5_b },
	{ 0x69, "BIT 5, C\0", 2, bit_5_c },
	{ 0x6A, "BIT 5, D\0", 2, bit_5_d },
	{ 0x6B, "BIT 5, E\0", 2, bit_5_e },
	{ 0x6C, "BIT 5, H\0", 2, bit_5_h },
	{ 0x6D, "BIT 5, L\0", 2, bit_5_l },
	{ 0x6E, "BIT 5, (HL)\0", 2, bit_5_at_hl },
	{ 0x6F, "BIT 5, A\0", 2, bit_5_a },

	{ 0x70, "BIT 6, B\0", 2, bit_6_b },
	{ 0x71, "BIT 6, C\0", 2, bit_6_c },
	{ 0x72, "BIT 6, D\0", 2, bit_6_d },
	{ 0x73, "BIT 6, E\0", 2, bit_6_e },
	{ 0x74, "BIT 6, H\0", 2, bit_6_h },
	{ 0x75, "BIT 6, L\0", 2, bit_6_l },
	{ 0x76, "BIT 6, (HL)\0", 2, bit_6_at_hl },
	{ 0x77, "BIT 6, A\0", 2, bit_6_a },
	{ 0x78, "BIT 7, B\0", 2, bit_7_b },
	{ 0x79, "BIT 7, C\0", 2, bit_7_c },
	{ 0x7A, "BIT 7, D\0", 2, bit_7_d },
	{ 0x7B, "BIT 7, E\0", 2, bit_7_e },
	{ 0x7C, "BIT 7, H\0", 2, bit_7_h },
	{ 0x7D, "BIT 7, L\0", 2, bit_7_l },
	{ 0x7E, "BIT 7, (HL)\0", 2, bit_7_at_hl },
	{ 0x7F, "BIT 7, A\0", 2, bit_7_a },

	{ 0x80, "RES 0, B\0", 2, res_0_b },
	{ 0x81, "RES 0, C\0", 2, res_0_c },
	{ 0x82, "RES 0, D\0", 2, res_0_d },
	{ 0x83, "RES 0, E\0", 2, res_0_e },
	{ 0x84, "RES 0, H\0", 2, res_0_h },
	{ 0x85, "RES 0, L\0", 2, res_0_l },
	{ 0x86, "RES 0, (HL)\0", 2, res_0_at_hl },
	{ 0x87, "RES 0, A\0", 2, res_0_a },
	{ 0x88, "RES 1, B\0", 2, res_1_b },
	{ 0x89, "RES 1, C\0", 2, res_1_c },
	{ 0x8A, "RES 1, D\0", 2, res_1_d },
	{ 0x8B, "RES 1, E\0", 2, res_1_e },
	{ 0x8C, "RES 1, H\0", 2, res_1_h },
	{ 0x8D, "RES 1, L\0", 2, res_1_l },
	{ 0x8E, "RES 1, (HL)\0", 2, res_1_at_hl },
	{ 0x8F, "RES 1, A\0", 2, res_1_a },

	{ 0x90, "RES 2, B\0", 2, res_2_b },
	{ 0x91, "RES 2, C\0", 2, res_2_c },
	{ 0x92, "RES 2, D\0", 2, res_2_d },
	{ 0x93, "RES 2, E\0", 2, res_2_e },
	{ 0x94, "RES 2, H\0", 2, res_2_h },
	{ 0x95, "RES 2, L\0", 2, res_2_l },
	{ 0x96, "RES 2, (HL)\0", 2, res_2_at_hl },
	{ 0x97, "RES 2, A\0", 2, res_2_a },
	{ 0x98, "RES 3, B\0", 2, res_3_b },
	{ 0x99, "RES 3, C\0", 2, res_3_c },
	{ 0x9A, "RES 3, D\0", 2, res_3_d },
	{ 0x9B, "RES 3, E\0", 2, res_3_e },
	{ 0x9C, "RES 3, H\0", 2, res_3_h },
	{ 0x9D, "RES 3, L\0", 2, res_3_l },
	{ 0x9E, "RES 3, (HL)\0", 2, res_3_at_hl },
	{ 0x9F, "RES 3, A\0", 2, res_3_a },

	{ 0xA0, "RES 4, B\0", 2, res_4_b },
	{ 0xA1, "RES 4, C\0", 2, res_4_c },
	{ 0xA2, "RES 4, D\0", 2, res_4_d },
	{ 0xA3, "RES 4, E\0", 2, res_4_e },
	{ 0xA4, "RES 4, H\0", 2, res_4_h },
	{ 0xA5, "RES 4, L\0", 2, res_4_l },
	{ 0xA6, "RES 4, (HL)\0", 2, res_4_at_hl },
	{ 0xA7, "RES 4, A\0", 2, res_4_a },
	{ 0xA8, "RES 5, B\0", 2, res_5_b },
	{ 0xA9, "RES 5, C\0", 2, res_5_c },
	{ 0xAA, "RES 5, D\0", 2, res_5_d },
	{ 0xAB, "RES 5, E\0", 2, res_5_e },
	{ 0xAC, "RES 5, H\0", 2, res_5_h },
	{ 0xAD, "RES 5, L\0", 2, res_5_l },
	{ 0xAE, "RES 5, (HL)\0", 2, res_5_at_hl },
	{ 0xAF, "RES 5, A\0", 2, res_5_a },

	{ 0xB0, "RES 6, B\0", 2, res_6_b },
	{ 0xB1, "RES 6, C\0", 2, res_6_c },
	{ 0xB2, "RES 6, D\0", 2, res_6_d },
	{ 0xB3, "RES 6, E\0", 2, res_6_e },
	{ 0xB4, "RES 6, H\0", 2, res_6_h },
	{ 0xB5, "RES 6, L\0", 2, res_6_l },
	{ 0xB6, "RES 6, (HL)\0", 2, res_6_at_hl },
	{ 0xB7, "RES 6, A\0", 2, res_6_a },
	{ 0xB8, "RES 7, B\0", 2, res_7_b },
	{ 0xB9, "RES 7, C\0", 2, res_7_c },
	{ 0xBA, "RES 7, D\0", 2, res_7_d },
	{ 0xBB, "RES 7, E\0", 2, res_7_e },
	{ 0xBC, "RES 7, H\0", 2, res_7_h },
	{ 0xBD, "RES 7, L\0", 2, res_7_l },
	{ 0xBE, "RES 7, (HL)\0", 2, res_7_at_hl },
	{ 0xBF, "RES 7, A\0", 2, res_7_a },

	{ 0xC0, "SET 0, B\0", 2, set_0_b },
	{ 0xC1, "SET 0, C\0", 2, set_0_c },
	{ 0xC2, "SET 0, D\0", 2, set_0_d },
	{ 0xC3, "SET 0, E\0", 2, set_0_e },
	{ 0xC4, "SET 0, H\0", 2, set_0_h },
	{ 0xC5, "SET 0, L\0", 2, set_0_l },
	{ 0xC6, "SET 0, (HL)\0", 2, set_0_at_hl },
	{ 0xC7, "SET 0, A\0", 2, set_0_a },
	{ 0xC8, "SET 1, B\0", 2, set_1_b },
	{ 0xC9, "SET 1, C\0", 2, set_1_c },
	{ 0xCA, "SET 1, D\0", 2, set_1_d },
	{ 0xCB, "SET 1, E\0", 2, set_1_e },
	{ 0xCC, "SET 1, H\0", 2, set_1_h },
	{ 0xCD, "SET 1, L\0", 2, set_1_l },
	{ 0xCE, "SET 1, (HL)\0", 2, set_1_at_hl },
	{ 0xCF, "SET 1, A\0", 2, set_1_a },

	{ 0xD0, "SET 2, B\0", 2, set_2_b },
	{ 0xD1, "SET 2, C\0", 2, set_2_c },
	{ 0xD2, "SET 2, D\0", 2, set_2_d },
	{ 0xD3, "SET 2, E\0", 2, set_2_e },
	{ 0xD4, "SET 2, H\0", 2, set_2_h },
	{ 0xD5, "SET 2, L\0", 2, set_2_l },
	{ 0xD6, "SET 2, (HL)\0", 2, set_2_at_hl },
	{ 0xD7, "SET 2, A\0", 2, set_2_a },
	{ 0xD8, "SET 3, B\0", 2, set_3_b },
	{ 0xD9, "SET 3, C\0", 2, set_3_c },
	{ 0xDA, "SET 3, D\0", 2, set_3_d },
	{ 0xDB, "SET 3, E\0", 2, set_3_e },
	{ 0xDC, "SET 3, H\0", 2, set_3_h },
	{ 0xDD, "SET 3, L\0", 2, set_3_l },
	{ 0xDE, "SET 3, (HL)\0", 2, set_3_at_hl },
	{ 0xDF, "SET 3, A\0", 2, set_3_a },

	{ 0xE0, "SET 4, B\0", 2, set_4_b },
	{ 0xE1, "SET 4, C\0", 2, set_4_c },
	{ 0xE2, "SET 4, D\0", 2, set_4_d },
	{ 0xE3, "SET 4, E\0", 2, set_4_e },
	{ 0xE4, "SET 4, H\0", 2, set_4_h },
	{ 0xE5, "SET 4, L\0", 2, set_4_l },
	{ 0xE6, "SET 4, (HL)\0", 2, set_4_at_hl },
	{ 0xE7, "SET 4, A\0", 2, set_4_a },
	{ 0xE8, "SET 5, B\0", 2, set_5_b },
	{ 0xE9, "SET 5, C\0", 2, set_5_c },
	{ 0xEA, "SET 5, D\0", 2, set_5_d },
	{ 0xEB, "SET 5, E\0", 2, set_5_e },
	{ 0xEC, "SET 5, H\0", 2, set_5_h },
	{ 0xED, "SET 5, L\0", 2, set_5_l },
	{ 0xEE, "SET 5, (HL)\0", 2, set_5_at_hl },
	{ 0xEF, "SET 5, A\0", 2, set_5_a },

	{ 0xF0, "SET 6, B\0", 2, set_6_b },
	{ 0xF1, "SET 6, C\0", 2, set_6_c },
	{ 0xF2, "SET 6, D\0", 2, set_6_d },
	{ 0xF3, "SET 6, E\0", 2, set_6_e },
	{ 0xF4, "SET 6, H\0", 2, set_6_h },
	{ 0xF5, "SET 6, L\0", 2, set_6_l },
	{ 0xF6, "SET 6, (HL)\0", 2, set_6_at_hl },
	{ 0xF7, "SET 6, A\0", 2, set_6_a },
	{ 0xF8, "SET 7, B\0", 2, set_7_b },
	{ 0xF9, "SET 7, C\0", 2, set_7_c },
	{ 0xFA, "SET 7, D\0", 2, set_7_d },
	{ 0xFB, "SET 7, E\0", 2, set_7_e },
	{ 0xFC, "SET 7, H\0", 2, set_7_h },
	{ 0xFD, "SET 7, L\0", 2, set_7_l },
	{ 0xFE, "SET 7, (HL)\0", 2, set_7_at_hl },
	{ 0xFF, "SET 7, A\0", 2, set_7_a },
};

void rlc_b() { rlc(&cpu.registers.b, &cpu.registers.f); }
void rlc_c() { rlc(&cpu.registers.c, &cpu.registers.f); }
void rlc_d() { rlc(&cpu.registers.d, &cpu.registers.f); }
void rlc_e() { rlc(&cpu.registers.e, &cpu.registers.f); }
void rlc_h() { rlc(&cpu.registers.h, &cpu.registers.f); }
void rlc_l() { rlc(&cpu.registers.l, &cpu.registers.f); }
void rlc_at_hl() {
	uint8_t reg = read_byte(cpu.registers.hl);
	rlc(&reg, &cpu.registers.f);
	write_byte(cpu.registers.hl, reg);
}
void rlc_a() { rlc(&cpu.registers.a, &cpu.registers.f); }
void rrc_b() { rrc(&cpu.registers.b, &cpu.registers.f); }
void rrc_c() { rrc(&cpu.registers.c, &cpu.registers.f); }
void rrc_d() { rrc(&cpu.registers.d, &cpu.registers.f); }
void rrc_e() { rrc(&cpu.registers.e, &cpu.registers.f); }
void rrc_h() { rrc(&cpu.registers.h, &cpu.registers.f); }
void rrc_l() { rrc(&cpu.registers.l, &cpu.registers.f); }
void rrc_at_hl() {
	uint8_t reg = read_byte(cpu.registers.hl);
	rrc(&reg, &cpu.registers.f);
	write_byte(cpu.registers.hl, reg);
}
void rrc_a() { rrc(&cpu.registers.a, &cpu.registers.f); }

//0x1x
void rl_b() { rl(&cpu.registers.b, &cpu.registers.f); }
void rl_c() { rl(&cpu.registers.c, &cpu.registers.f); }
void rl_d() { rl(&cpu.registers.d, &cpu.registers.f); }
void rl_e() { rl(&cpu.registers.e, &cpu.registers.f); }
void rl_h() { rl(&cpu.registers.h, &cpu.registers.f); }
void rl_l() { rl(&cpu.registers.l, &cpu.registers.f); }
void rl_at_hl() {
	uint8_t reg = read_byte(cpu.registers.hl);
	rl(&reg, &cpu.registers.f);
	write_byte(cpu.registers.hl, reg);
}
void rl_a() { rl(&cpu.registers.a, &cpu.registers.f); }
void rr_b() { rr(&cpu.registers.b, &cpu.registers.f); }
void rr_c() { rr(&cpu.registers.c, &cpu.registers.f); }
void rr_d() { rr(&cpu.registers.d, &cpu.registers.f); }
void rr_e() { rr(&cpu.registers.e, &cpu.registers.f); }
void rr_h() { rr(&cpu.registers.h, &cpu.registers.f); }
void rr_l() { rr(&cpu.registers.l, &cpu.registers.f); }
void rr_at_hl() {
	uint8_t reg = read_byte(cpu.registers.hl);
	rr(&reg, &cpu.registers.f);
	write_byte(cpu.registers.hl, reg);
}
void rr_a() { rr(&cpu.registers.a, &cpu.registers.f); }

void sla_b() { sla(&cpu.registers.b, &cpu.registers.f); }
void sla_c() { sla(&cpu.registers.c, &cpu.registers.f); }
void sla_d() { sla(&cpu.registers.d, &cpu.registers.f); }
void sla_e() { sla(&cpu.registers.e, &cpu.registers.f); }
void sla_h() { sla(&cpu.registers.h, &cpu.registers.f); }
void sla_l() { sla(&cpu.registers.l, &cpu.registers.f); }
void sla_at_hl() {
	uint8_t reg = read_byte(cpu.registers.hl);
	sla(&reg, &cpu.registers.f);
	write_byte(cpu.registers.hl, reg);
}
void sla_a() { sla(&cpu.registers.a, &cpu.registers.f); }
void sra_b() { sra(&cpu.registers.b, &cpu.registers.f); }
void sra_c() { sra(&cpu.registers.c, &cpu.registers.f); }
void sra_d() { sra(&cpu.registers.d, &cpu.registers.f); }
void sra_e() { sra(&cpu.registers.e, &cpu.registers.f); }
void sra_h() { sra(&cpu.registers.h, &cpu.registers.f); }
void sra_l() { sra(&cpu.registers.l, &cpu.registers.f); }
void sra_at_hl() {
	uint8_t reg = read_byte(cpu.registers.hl);
	sra(&reg, &cpu.registers.f);
	write_byte(cpu.registers.hl, reg);
}
void sra_a() { sra(&cpu.registers.a, &cpu.registers.f); }

void swap_b() { swap(&cpu.registers.b, &cpu.registers.f); }
void swap_c() { swap(&cpu.registers.c, &cpu.registers.f); }
void swap_d() { swap(&cpu.registers.d, &cpu.registers.f); }
void swap_e() { swap(&cpu.registers.e, &cpu.registers.f); }
void swap_h() { swap(&cpu.registers.h, &cpu.registers.f); }
void swap_l() { swap(&cpu.registers.l, &cpu.registers.f); }
void swap_at_hl() {
	uint8_t reg = read_byte(cpu.registers.hl);
	swap(&reg, &cpu.registers.f);
	write_byte(cpu.registers.hl, reg);
}
void swap_a() { swap(&cpu.registers.a, &cpu.registers.f); }
void srl_b() { srl(&cpu.registers.b, &cpu.registers.f); }
void srl_c() { srl(&cpu.registers.c, &cpu.registers.f); }
void srl_d() { srl(&cpu.registers.d, &cpu.registers.f); }
void srl_e() { srl(&cpu.registers.e, &cpu.registers.f); }
void srl_h() { srl(&cpu.registers.h, &cpu.registers.f); }
void srl_l() { srl(&cpu.registers.l, &cpu.registers.f); }
void srl_at_hl() {
	uint8_t reg = read_byte(cpu.registers.hl);
	srl(&reg, &cpu.registers.f);
	write_byte(cpu.registers.hl, reg);
}
void srl_a() { srl(&cpu.registers.a, &cpu.registers.f); }

void bit_0_b() { bit(cpu.registers.b, &cpu.registers.f, 0); }
void bit_0_c() { bit(cpu.registers.c, &cpu.registers.f, 0); }
void bit_0_d() { bit(cpu.registers.d, &cpu.registers.f, 0); }
void bit_0_e() { bit(cpu.registers.e, &cpu.registers.f, 0); }
void bit_0_h() { bit(cpu.registers.h, &cpu.registers.f, 0); }
void bit_0_l() { bit(cpu.registers.l, &cpu.registers.f, 0); }
void bit_0_at_hl() {
	uint8_t reg = read_byte(cpu.registers.hl);
	bit(reg, &cpu.registers.f, 0);
}
void bit_0_a() { bit(cpu.registers.a, &cpu.registers.f, 0); }
void bit_1_b() { bit(cpu.registers.b, &cpu.registers.f, 1); }
void bit_1_c() { bit(cpu.registers.c, &cpu.registers.f, 1); }
void bit_1_d() { bit(cpu.registers.d, &cpu.registers.f, 1); }
void bit_1_e() { bit(cpu.registers.e, &cpu.registers.f, 1); }
void bit_1_h() { bit(cpu.registers.h, &cpu.registers.f, 1); }
void bit_1_l() { bit(cpu.registers.l, &cpu.registers.f, 1); }
void bit_1_at_hl() {
	uint8_t reg = read_byte(cpu.registers.hl);
	bit(reg, &cpu.registers.f, 1);
}
void bit_1_a() { bit(cpu.registers.a, &cpu.registers.f, 1); }

void bit_2_b() { bit(cpu.registers.b, &cpu.registers.f, 2); }
void bit_2_c() { bit(cpu.registers.c, &cpu.registers.f, 2); }
void bit_2_d() { bit(cpu.registers.d, &cpu.registers.f, 2); }
void bit_2_e() { bit(cpu.registers.e, &cpu.registers.f, 2); }
void bit_2_h() { bit(cpu.registers.h, &cpu.registers.f, 2); }
void bit_2_l() { bit(cpu.registers.l, &cpu.registers.f, 2); }
void bit_2_at_hl() {
	uint8_t reg = read_byte(cpu.registers.hl);
	bit(reg, &cpu.registers.f, 2);
}
void bit_2_a() { bit(cpu.registers.a, &cpu.registers.f, 2); }
void bit_3_b() { bit(cpu.registers.b, &cpu.registers.f, 3); }
void bit_3_c() { bit(cpu.registers.c, &cpu.registers.f, 3); }
void bit_3_d() { bit(cpu.registers.d, &cpu.registers.f, 3); }
void bit_3_e() { bit(cpu.registers.e, &cpu.registers.f, 3); }
void bit_3_h() { bit(cpu.registers.h, &cpu.registers.f, 3); }
void bit_3_l() { bit(cpu.registers.l, &cpu.registers.f, 3); }
void bit_3_at_hl() {
	uint8_t reg = read_byte(cpu.registers.hl);
	bit(reg, &cpu.registers.f, 3);
}
void bit_3_a() { bit(cpu.registers.a, &cpu.registers.f, 3); }

void bit_4_b() { bit(cpu.registers.b, &cpu.registers.f, 4); }
void bit_4_c() { bit(cpu.registers.c, &cpu.registers.f, 4); }
void bit_4_d() { bit(cpu.registers.d, &cpu.registers.f, 4); }
void bit_4_e() { bit(cpu.registers.e, &cpu.registers.f, 4); }
void bit_4_h() { bit(cpu.registers.h, &cpu.registers.f, 4); }
void bit_4_l() { bit(cpu.registers.l, &cpu.registers.f, 4); }
void bit_4_at_hl() {
	uint8_t reg = read_byte(cpu.registers.hl);
	bit(reg, &cpu.registers.f, 4);
}
void bit_4_a() { bit(cpu.registers.a, &cpu.registers.f, 4); }
void bit_5_b() { bit(cpu.registers.b, &cpu.registers.f, 5); }
void bit_5_c() { bit(cpu.registers.c, &cpu.registers.f, 5); }
void bit_5_d() { bit(cpu.registers.d, &cpu.registers.f, 5); }
void bit_5_e() { bit(cpu.registers.e, &cpu.registers.f, 5); }
void bit_5_h() { bit(cpu.registers.h, &cpu.registers.f, 5); }
void bit_5_l() { bit(cpu.registers.l, &cpu.registers.f, 5); }
void bit_5_at_hl() {
	uint8_t reg = read_byte(cpu.registers.hl);
	bit(reg, &cpu.registers.f, 5);
}
void bit_5_a() { bit(cpu.registers.a, &cpu.registers.f, 5); }

void bit_6_b() { bit(cpu.registers.b, &cpu.registers.f, 6); }
void bit_6_c() { bit(cpu.registers.c, &cpu.registers.f, 6); }
void bit_6_d() { bit(cpu.registers.d, &cpu.registers.f, 6); }
void bit_6_e() { bit(cpu.registers.e, &cpu.registers.f, 6); }
void bit_6_h() { bit(cpu.registers.h, &cpu.registers.f, 6); }
void bit_6_l() { bit(cpu.registers.l, &cpu.registers.f, 6); }
void bit_6_at_hl() {
	uint8_t reg = read_byte(cpu.registers.hl);
	bit(reg, &cpu.registers.f, 6);
}
void bit_6_a() { bit(cpu.registers.a, &cpu.registers.f, 6); }
void bit_7_b() { bit(cpu.registers.b, &cpu.registers.f, 7); }
void bit_7_c() { bit(cpu.registers.c, &cpu.registers.f, 7); }
void bit_7_d() { bit(cpu.registers.d, &cpu.registers.f, 7); }
void bit_7_e() { bit(cpu.registers.e, &cpu.registers.f, 7); }
void bit_7_h() { bit(cpu.registers.h, &cpu.registers.f, 7); }
void bit_7_l() { bit(cpu.registers.l, &cpu.registers.f, 7); }
void bit_7_at_hl() {
	uint8_t reg = read_byte(cpu.registers.hl);
	bit(reg, &cpu.registers.f, 7);
}
void bit_7_a() { bit(cpu.registers.a, &cpu.registers.f, 7); }

void res_0_b() { res(&cpu.registers.b, 0); }
void res_0_c() { res(&cpu.registers.c, 0); }
void res_0_d() { res(&cpu.registers.d, 0); }
void res_0_e() { res(&cpu.registers.e, 0); }
void res_0_h() { res(&cpu.registers.h, 0); }
void res_0_l() { res(&cpu.registers.l, 0); }
void res_0_at_hl() {
	uint8_t reg = read_byte(cpu.registers.hl);
	res(&reg, 0);
	write_byte(cpu.registers.hl, reg);
}
void res_0_a() { res(&cpu.registers.a, 0); }
void res_1_b() { res(&cpu.registers.b, 1); }
void res_1_c() { res(&cpu.registers.c, 1); }
void res_1_d() { res(&cpu.registers.d, 1); }
void res_1_e() { res(&cpu.registers.e, 1); }
void res_1_h() { res(&cpu.registers.h, 1); }
void res_1_l() { res(&cpu.registers.l, 1); }
void res_1_at_hl() {
	uint8_t reg = read_byte(cpu.registers.hl);
	res(&reg, 1);
	write_byte(cpu.registers.hl, reg);
}
void res_1_a() { res(&cpu.registers.a, 1); }

void res_2_b() { res(&cpu.registers.b, 2); }
void res_2_c() { res(&cpu.registers.c, 2); }
void res_2_d() { res(&cpu.registers.d, 2); }
void res_2_e() { res(&cpu.registers.e, 2); }
void res_2_h() { res(&cpu.registers.h, 2); }
void res_2_l() { res(&cpu.registers.l, 2); }
void res_2_at_hl() {
	uint8_t reg = read_byte(cpu.registers.hl);
	res(&reg, 2);
	write_byte(cpu.registers.hl, reg);
}
void res_2_a() { res(&cpu.registers.a, 2); }
void res_3_b() { res(&cpu.registers.b, 3); }
void res_3_c() { res(&cpu.registers.c, 3); }
void res_3_d() { res(&cpu.registers.d, 3); }
void res_3_e() { res(&cpu.registers.e, 3); }
void res_3_h() { res(&cpu.registers.h, 3); }
void res_3_l() { res(&cpu.registers.l, 3); }
void res_3_at_hl() {
	uint8_t reg = read_byte(cpu.registers.hl);
	res(&reg, 3);
	write_byte(cpu.registers.hl, reg);
}
void res_3_a() { res(&cpu.registers.a, 3); }

void res_4_b() { res(&cpu.registers.b, 4); }
void res_4_c() { res(&cpu.registers.c, 4); }
void res_4_d() { res(&cpu.registers.d, 4); }
void res_4_e() { res(&cpu.registers.e, 4); }
void res_4_h() { res(&cpu.registers.h, 4); }
void res_4_l() { res(&cpu.registers.l, 4); }
void res_4_at_hl() {
	uint8_t reg = read_byte(cpu.registers.hl);
	res(&reg, 4);
	write_byte(cpu.registers.hl, reg);
}
void res_4_a() { res(&cpu.registers.a, 4); }
void res_5_b() { res(&cpu.registers.b, 5); }
void res_5_c() { res(&cpu.registers.c, 5); }
void res_5_d() { res(&cpu.registers.d, 5); }
void res_5_e() { res(&cpu.registers.e, 5); }
void res_5_h() { res(&cpu.registers.h, 5); }
void res_5_l() { res(&cpu.registers.l, 5); }
void res_5_at_hl() {
	uint8_t reg = read_byte(cpu.registers.hl);
	res(&reg, 5);
	write_byte(cpu.registers.hl, reg);
}
void res_5_a() { res(&cpu.registers.a, 5); }

//0xB0
void res_6_b() { res(&cpu.registers.b, 6); }
void res_6_c() { res(&cpu.registers.c, 6); }
void res_6_d() { res(&cpu.registers.d, 6); }
void res_6_e() { res(&cpu.registers.e, 6); }
void res_6_h() { res(&cpu.registers.h, 6); }
void res_6_l() { res(&cpu.registers.l, 6); }
void res_6_at_hl() {
	uint8_t reg = read_byte(cpu.registers.hl);
	res(&reg, 6);
	write_byte(cpu.registers.hl, reg);
}
void res_6_a() { res(&cpu.registers.a, 6); }
void res_7_b() { res(&cpu.registers.b, 7); }
void res_7_c() { res(&cpu.registers.c, 7); }
void res_7_d() { res(&cpu.registers.d, 7); }
void res_7_e() { res(&cpu.registers.e, 7); }
void res_7_h() { res(&cpu.registers.h, 7); }
void res_7_l() { res(&cpu.registers.l, 7); }
void res_7_at_hl() {
	uint8_t reg = read_byte(cpu.registers.hl);
	res(&reg, 7);
	write_byte(cpu.registers.hl, reg);
}
void res_7_a() { res(&cpu.registers.a, 7); }

//0xC0
void set_0_b() { set(&cpu.registers.b, 0); }
void set_0_c() { set(&cpu.registers.c, 0); }
void set_0_d() { set(&cpu.registers.d, 0); }
void set_0_e() { set(&cpu.registers.e, 0); }
void set_0_h() { set(&cpu.registers.h, 0); }
void set_0_l() { set(&cpu.registers.l, 0); }
void set_0_at_hl() {
	uint8_t reg = read_byte(cpu.registers.hl);
	set(&reg, 0);
	write_byte(cpu.registers.hl, reg);
}
void set_0_a() { set(&cpu.registers.a, 0); }
void set_1_b() { set(&cpu.registers.b, 1); }
void set_1_c() { set(&cpu.registers.c, 1); }
void set_1_d() { set(&cpu.registers.d, 1); }
void set_1_e() { set(&cpu.registers.e, 1); }
void set_1_h() { set(&cpu.registers.h, 1); }
void set_1_l() { set(&cpu.registers.l, 1); }
void set_1_at_hl() {
	uint8_t reg = read_byte(cpu.registers.hl);
	set(&reg, 1);
	write_byte(cpu.registers.hl, reg);
}
void set_1_a() { set(&cpu.registers.a, 1); }

void set_2_b() { set(&cpu.registers.b, 2); }
void set_2_c() { set(&cpu.registers.c, 2); }
void set_2_d() { set(&cpu.registers.d, 2); }
void set_2_e() { set(&cpu.registers.e, 2); }
void set_2_h() { set(&cpu.registers.h, 2); }
void set_2_l() { set(&cpu.registers.l, 2); }
void set_2_at_hl() {
	uint8_t reg = read_byte(cpu.registers.hl);
	set(&reg, 2);
	write_byte(cpu.registers.hl, reg);
}
void set_2_a() { set(&cpu.registers.a, 2); }
void set_3_b() { set(&cpu.registers.b, 3); }
void set_3_c() { set(&cpu.registers.c, 3); }
void set_3_d() { set(&cpu.registers.d, 3); }
void set_3_e() { set(&cpu.registers.e, 3); }
void set_3_h() { set(&cpu.registers.h, 3); }
void set_3_l() { set(&cpu.registers.l, 3); }
void set_3_at_hl() {
	uint8_t reg = read_byte(cpu.registers.hl);
	set(&reg, 3);
	write_byte(cpu.registers.hl, reg);
}
void set_3_a() { set(&cpu.registers.a, 3); }

void set_4_b() { set(&cpu.registers.b, 4); }
void set_4_c() { set(&cpu.registers.c, 4); }
void set_4_d() { set(&cpu.registers.d, 4); }
void set_4_e() { set(&cpu.registers.e, 4); }
void set_4_h() { set(&cpu.registers.h, 4); }
void set_4_l() { set(&cpu.registers.l, 4); }
void set_4_at_hl() {
	uint8_t reg = read_byte(cpu.registers.hl);
	set(&reg, 4);
	write_byte(cpu.registers.hl, reg);
}
void set_4_a() { set(&cpu.registers.a, 4); }
void set_5_b() { set(&cpu.registers.b, 5); }
void set_5_c() { set(&cpu.registers.c, 5); }
void set_5_d() { set(&cpu.registers.d, 5); }
void set_5_e() { set(&cpu.registers.e, 5); }
void set_5_h() { set(&cpu.registers.h, 5); }
void set_5_l() { set(&cpu.registers.l, 5); }
void set_5_at_hl() {
	uint8_t reg = read_byte(cpu.registers.hl);
	set(&reg, 5);
	write_byte(cpu.registers.hl, reg);
}
void set_5_a() { set(&cpu.registers.a, 5); }

void set_6_b() { set(&cpu.registers.b, 6); }
void set_6_c() { set(&cpu.registers.c, 6); }
void set_6_d() { set(&cpu.registers.d, 6); }
void set_6_e() { set(&cpu.registers.e, 6); }
void set_6_h() { set(&cpu.registers.h, 6); }
void set_6_l() { set(&cpu.registers.l, 6); }
void set_6_at_hl() {
	uint8_t reg = read_byte(cpu.registers.hl);
	set(&reg, 6);
	write_byte(cpu.registers.hl, reg);
}
void set_6_a() { set(&cpu.registers.a, 6); }
void set_7_b() { set(&cpu.registers.b, 7); }
void set_7_c() { set(&cpu.registers.c, 7); }
void set_7_d() { set(&cpu.registers.d, 7); }
void set_7_e() { set(&cpu.registers.e, 7); }
void set_7_h() { set(&cpu.registers.h, 7); }
void set_7_l() { set(&cpu.registers.l, 7); }
void set_7_at_hl() {
	uint8_t reg = read_byte(cpu.registers.hl);
	set(&reg, 7);
	write_byte(cpu.registers.hl, reg);
}
void set_7_a() { set(&cpu.registers.a, 7); }