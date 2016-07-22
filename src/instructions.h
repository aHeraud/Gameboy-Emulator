#ifndef INSTRUCTIONS_H
#define INSTRUCTUINS_H

#include<stdint.h>

typedef struct instruction_t {
	uint8_t opcode;
	char* assembly;
	uint8_t length;
	void* func;
} instruction_t;

const extern instruction_t instructions[256];
const extern instruction_t extended_instructions[256];
extern uint8_t instruction_cycles[256];
extern uint8_t extended_instruction_cycles[256];

/*Instruction functions*/

//$0x
void nop();
void ld_bc_d16(uint16_t op16);
void ld_at_bc_a();
void inc_bc();
void inc_b();
void dec_b();
void ld_b_d8(uint8_t op8);
void rlca();
void ld_at_a16_sp(uint16_t op16);
void add_hl_bc();
void ld_a_at_bc();
void dec_bc();
void inc_c();
void dec_c();
void ld_c_d8(uint8_t op8);
void rrca();

//$1x
void stop();
void ld_de_d16(uint16_t op16);
void ld_at_de_a();
void inc_de();
void inc_d();
void dec_d();
void ld_d_d8(uint8_t op8);
void rla();
void jr_r8(uint8_t op8);
void add_hl_de();
void ld_a_at_de();
void dec_de();
void inc_e();
void dec_e();
void ld_e_d8(uint8_t op8);
void rra();

//$2x
void jr_nz_r8(uint8_t op8);
void ld_hl_d16(uint16_t op16);
void ldi_at_hl_a();
void inc_hl();
void inc_h();
void dec_h();
void ld_h_d8(uint8_t op8);
void daa();
void jr_z_r8(uint8_t);
void add_hl_hl();
void ldi_a_at_hl();
void dec_hl();
void inc_l();
void dec_l();
void ld_l_d8(uint8_t op8);
void cpl();

//$3x
void jr_nc_r8(uint8_t op8);
void ld_sp_d16(uint16_t op16);
void ldd_at_hl_a();
void inc_sp();
void inc_at_hl();
void dec_at_hl();
void ld_at_hl_d8(uint8_t op8);
void scf();
void jr_c_r8(uint8_t op8);
void add_hl_sp();
void ldd_a_at_hl();
void dec_sp();
void inc_a();
void dec_a();
void ld_a_d8(uint8_t op8);
void ccf();

//$4x
void ld_b_b();
void ld_b_c();
void ld_b_d();
void ld_b_e();
void ld_b_f();
void ld_b_h();
void ld_b_l();
void ld_b_at_hl();
void ld_b_a();
void ld_c_b();
void ld_c_c();
void ld_c_d();
void ld_c_e();
void ld_c_h();
void ld_c_l();
void ld_c_at_hl();
void ld_c_a();

//$5x
void ld_d_b();
void ld_d_c();
void ld_d_d();
void ld_d_e();
void ld_d_f();
void ld_d_h();
void ld_d_l();
void ld_d_at_hl();
void ld_d_a();
void ld_e_b();
void ld_e_c();
void ld_e_d();
void ld_e_e();
void ld_e_h();
void ld_e_l();
void ld_e_at_hl();
void ld_e_a();

//$6x
void ld_h_b();
void ld_h_c();
void ld_h_d();
void ld_h_e();
void ld_h_f();
void ld_h_h();
void ld_h_l();
void ld_h_at_hl();
void ld_h_a();
void ld_l_b();
void ld_l_c();
void ld_l_d();
void ld_l_e();
void ld_l_h();
void ld_l_l();
void ld_l_at_hl();
void ld_l_a();

//$7x
void ld_at_hl_b();
void ld_at_hl_c();
void ld_at_hl_d();
void ld_at_hl_e();
void halt();
void ld_at_hl_h();
void ld_at_hl_l();
void ld_at_hl_a();
void ld_a_b();
void ld_a_c();
void ld_a_d();
void ld_a_e();
void ld_a_h();
void ld_a_l();
void ld_a_at_hl();
void ld_a_a();

//$8x
void add_a_b();
void add_a_c();
void add_a_d();
void add_a_e();
void add_a_h();
void add_a_l();
void add_a_at_hl();
void add_a_a();
void adc_a_b();
void adc_a_c();
void adc_a_d();
void adc_a_e();
void adc_a_h();
void adc_a_l();
void adc_a_at_hl();
void adc_a_a();

//$9x
void sub_b();
void sub_c();
void sub_d();
void sub_e();
void sub_h();
void sub_l();
void sub_at_hl();
void sub_a();
void sbc_a_b();
void sbc_a_c();
void sbc_a_d();
void sbc_a_e();
void sbc_a_h();
void sbc_a_l();
void sbc_a_at_hl();
void sbc_a_a();

//$Ax
void and_b();
void and_c();
void and_d();
void and_e();
void and_h();
void and_l();
void and_at_hl();
void and_a();
void xor_b();
void xor_c();
void xor_d();
void xor_e();
void xor_h();
void xor_l();
void xor_at_hl();
void xor_a();

//$Bx
void or_b();
void or_c();
void or_d();
void or_e();
void or_h();
void or_l();
void or_at_hl();
void or_a();
void cp_b();
void cp_c();
void cp_d();
void cp_e();
void cp_h();
void cp_l();
void cp_at_hl();
void cp_a();

//$Cx
void ret_nz();
void pop_bc();
void jp_nz_a16(uint16_t op16);
void jp_a16(uint16_t op16);
void call_nz_a16(uint16_t op16);
void push_bc();
void add_a_d8(uint8_t op8);
void rst_00h();
void ret_z();
void ret();
void jp_z_a16(uint16_t op16);
//PREFIX CB: INVALID
void call_z_a16(uint16_t op16);
void call_a16(uint16_t op16);
void adc_a_d8(uint8_t op8);
void rst_08h();

//$Dx
void ret_nc();
void pop_de();
void jp_nc_a16(uint16_t op16);
//0xD3: INVALID
void call_nc_a16(uint16_t op16);
void push_de();
void sub_d8(uint8_t op8);
void rst_10h();
void ret_c();
void reti();
void jp_c_a16(uint16_t op16);
//0xDB: INVALID
void call_c_a16(uint16_t op16);
//0xDD: INVALID
void sbc_a_d8(uint8_t op8);
void rst_18h();

//$Ex
void ldh_at_a8_a(uint8_t op8);
void pop_hl();
void ld_at_c_a();
//0xE3: INVALID
//0xE4: INVALID
void push_hl();
void and_d8(uint8_t op8);
void rst_20h();
void add_sp_r8(uint8_t op8);
void jp_hl();
void ld_at_a16_a(uint16_t op16);
//0xEB: INVALID
//0xEC: INVALID
//0xED: INVALID
void xor_d8(uint8_t op8);
void rst_28h();

//$Fx
void ldh_a_at_a8(uint8_t op8);
void pop_af();
void ld_a_at_c();
void di();
//0xF4: INVALID
void push_af();
void or_d8(uint8_t op8);
void rst_30h();
void ld_hl_sp_plus_r8(uint8_t op8);
void ld_sp_hl();
void ld_a_at_a16(uint16_t op16);
void ei();
//0xFC: INVALID
//0xFD: INVALID
void cp_d8(uint8_t op8);
void rst_38h();


/***********************/
/*Extended Instructions*/
/***********************/

//0x0x
void rlc_b();
void rlc_c();
void rlc_d();
void rlc_e();
void rlc_h();
void rlc_l();
void rlc_at_hl();
void rlc_a();
void rrc_b();
void rrc_c();
void rrc_d();
void rrc_e();
void rrc_h();
void rrc_l();
void rrc_at_hl();
void rrc_a();

//0x1x
void rl_b();
void rl_c();
void rl_d();
void rl_e();
void rl_h();
void rl_l();
void rl_at_hl();
void rl_a();
void rr_b();
void rr_c();
void rr_d();
void rr_e();
void rr_h();
void rr_l();
void rr_at_hl();
void rr_a();

void sla_b();
void sla_c();
void sla_d();
void sla_e();
void sla_h();
void sla_l();
void sla_at_hl();
void sla_a();
void sra_b();
void sra_c();
void sra_d();
void sra_e();
void sra_h();
void sra_l();
void sra_at_hl();
void sra_a();

void swap_b();
void swap_c();
void swap_d();
void swap_e();
void swap_h();
void swap_l();
void swap_at_hl();
void swap_a();
void srl_b();
void srl_c();
void srl_d();
void srl_e();
void srl_h();
void srl_l();
void srl_at_hl();
void srl_a();

void bit_0_b();
void bit_0_c();
void bit_0_d();
void bit_0_e();
void bit_0_h();
void bit_0_l();
void bit_0_at_hl();
void bit_0_a();
void bit_1_b();
void bit_1_c();
void bit_1_d();
void bit_1_e();
void bit_1_h();
void bit_1_l();
void bit_1_at_hl();
void bit_1_a();

void bit_2_b();
void bit_2_c();
void bit_2_d();
void bit_2_e();
void bit_2_h();
void bit_2_l();
void bit_2_at_hl();
void bit_2_a();
void bit_3_b();
void bit_3_c();
void bit_3_d();
void bit_3_e();
void bit_3_h();
void bit_3_l();
void bit_3_at_hl();
void bit_3_a();

void bit_4_b();
void bit_4_c();
void bit_4_d();
void bit_4_e();
void bit_4_h();
void bit_4_l();
void bit_4_at_hl();
void bit_4_a();
void bit_5_b();
void bit_5_c();
void bit_5_d();
void bit_5_e();
void bit_5_h();
void bit_5_l();
void bit_5_at_hl();
void bit_5_a();

void bit_6_b();
void bit_6_c();
void bit_6_d();
void bit_6_e();
void bit_6_h();
void bit_6_l();
void bit_6_at_hl();
void bit_6_a();
void bit_7_b();
void bit_7_c();
void bit_7_d();
void bit_7_e();
void bit_7_h();
void bit_7_l();
void bit_7_at_hl();
void bit_7_a();

void res_0_b();
void res_0_c();
void res_0_d();
void res_0_e();
void res_0_h();
void res_0_l();
void res_0_at_hl();
void res_0_a();
void res_1_b();
void res_1_c();
void res_1_d();
void res_1_e();
void res_1_h();
void res_1_l();
void res_1_at_hl();
void res_1_a();

void res_2_b();
void res_2_c();
void res_2_d();
void res_2_e();
void res_2_h();
void res_2_l();
void res_2_at_hl();
void res_2_a();
void res_3_b();
void res_3_c();
void res_3_d();
void res_3_e();
void res_3_h();
void res_3_l();
void res_3_at_hl();
void res_3_a();

void res_4_b();
void res_4_c();
void res_4_d();
void res_4_e();
void res_4_h();
void res_4_l();
void res_4_at_hl();
void res_4_a();
void res_5_b();
void res_5_c();
void res_5_d();
void res_5_e();
void res_5_h();
void res_5_l();
void res_5_at_hl();
void res_5_a();

void res_6_b();
void res_6_c();
void res_6_d();
void res_6_e();
void res_6_h();
void res_6_l();
void res_6_at_hl();
void res_6_a();
void res_7_b();
void res_7_c();
void res_7_d();
void res_7_e();
void res_7_h();
void res_7_l();
void res_7_at_hl();
void res_7_a();

void set_0_b();
void set_0_c();
void set_0_d();
void set_0_e();
void set_0_h();
void set_0_l();
void set_0_at_hl();
void set_0_a();
void set_1_b();
void set_1_c();
void set_1_d();
void set_1_e();
void set_1_h();
void set_1_l();
void set_1_at_hl();
void set_1_a();

void set_2_b();
void set_2_c();
void set_2_d();
void set_2_e();
void set_2_h();
void set_2_l();
void set_2_at_hl();
void set_2_a();
void set_3_b();
void set_3_c();
void set_3_d();
void set_3_e();
void set_3_h();
void set_3_l();
void set_3_at_hl();
void set_3_a();

void set_4_b();
void set_4_c();
void set_4_d();
void set_4_e();
void set_4_h();
void set_4_l();
void set_4_at_hl();
void set_4_a();
void set_5_b();
void set_5_c();
void set_5_d();
void set_5_e();
void set_5_h();
void set_5_l();
void set_5_at_hl();
void set_5_a();

void set_6_b();
void set_6_c();
void set_6_d();
void set_6_e();
void set_6_h();
void set_6_l();
void set_6_at_hl();
void set_6_a();
void set_7_b();
void set_7_c();
void set_7_d();
void set_7_e();
void set_7_h();
void set_7_l();
void set_7_at_hl();
void set_7_a();

#endif