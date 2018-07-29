#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "tables.h"
#include "translate_utils.h"
#include "translate.h"

/* Writes instructions during the assembler's first pass to OUTPUT. The case
   for general instructions has already been completed, but you need to write
   code to translate the li, bge and move pseudoinstructions. Your pseudoinstruction 
   expansions should not have any side effects.

   NAME is the name of the instruction, ARGS is an array of the arguments, and
   NUM_ARGS specifies the number of items in ARGS.

   Error checking for regular instructions are done in pass two. However, for
   pseudoinstructions, you must make sure that ARGS contains the correct number
   of arguments. You do NOT need to check whether the registers / label are 
   valid, since that will be checked in part two.

   Also for li:
	- make sure that the number is representable by 32 bits. (Hint: the number 
		can be both signed or unsigned).
	- if the immediate can fit in the imm field of an addiu instruction, then
		expand li into a single addiu instruction. Otherwise, expand it into 
		a lui-ori pair.

   And for bge and move:
	- your expansion should use the fewest number of instructions possible.

   MARS has slightly different translation rules for li, and it allows numbers
   larger than the largest 32 bit number to be loaded with li. You should follow
   the above rules if MARS behaves differently.

   Use fprintf() to write. If writing multiple instructions, make sure that 
   each instruction is on a different line.

   Returns the number of instructions written (so 0 if there were any errors).
 */
unsigned write_pass_one(FILE* output, const char* name, char** args, int num_args) {
  /* DECLARATIONS */
	char* sub_args[3];
	char buf[100];
	if (!output || !name || !args) return 0; /* Basic error checking */
  /* Expand pseudo `li` */
	if (strcmp(name, "li") == 0) {
		long int imm; /* The immdiate */
		int err; /* return state of translate */
		if (num_args != 2) return 0; /* Basic error checking */
	  /* Translate */
		err = translate_num(&imm, args[1], 4294967295, -2147483648); /* Notice the range */
		if (err == -1) return 0; /* Translate fails */
	  /* If in range of 16-bits, expand to `addiu` */
		if ((-32768 <= imm) && (imm <= 32767)) {
			sub_args[0] = args[0]; /* Assign sub_args */
			sub_args[1] = "$0";
			sub_args[2] = args[1];
			write_inst_string(output, "addiu", sub_args, 3); /* Write */
			return 1; /* One line written */
	  /* Else in range of 32-bits, expand to `lui` and `ori` */
		} else {
			sprintf(buf, "%u", (uint16_t)(imm>>16)); /* Upper 16-bits to `lui` */
			sub_args[0] = "$at"; /* Assign sub_args */
			sub_args[1] = buf;
			write_inst_string(output, "lui", sub_args, 2); /* Write */
			sprintf(buf, "%ld", (imm & 0xffff)); /* Lower 16-bits to `ori` */
			sub_args[0] = args[0]; /* Assign sub_args */
			sub_args[1] = "$at";
			sub_args[2] = buf;
			write_inst_string(output, "ori", sub_args, 3); /* Write */
			return 2; /* Two lines written */
		}
  /* Expand pseudo `bge` */
	} else if (strcmp(name, "bge") == 0) {
		if (num_args != 3) return 0; /* Basic error checking */
		sub_args[0] = "$at"; /* Assign sub_args */
		sub_args[1] = args[0];
		sub_args[2] = args[1];
		write_inst_string(output, "slt", sub_args, 3); /* Write */
		sub_args[0] = "$at"; /* Assign sub_args */
		sub_args[1] = "$0";
		sub_args[2] = args[2];
		write_inst_string(output, "beq", sub_args, 3); /* Write */
		return 2; /* Two lines written */
  /* Expand pseudo `move` */
	} else if (strcmp(name, "move") == 0 ) {
		if (num_args != 2) return 0; /* Basic error checking */
		sub_args[0] = args[0]; /* Assign sub_args */
		sub_args[1] = "$0";
		sub_args[2] = args[1];
		write_inst_string(output, "addu", sub_args, 3); /* Write */
		return 1; /* One line written */
  /* Non-pseudo instructions, directly write */
	} else {
		write_inst_string(output, name, args, num_args);
		return 1; /* One line written */
	}
}

/* Writes the instruction in hexadecimal format to OUTPUT during pass #2.
   
   NAME is the name of the instruction, ARGS is an array of the arguments, and
   NUM_ARGS specifies the number of items in ARGS. 

   The symbol table (SYMTBL) is given for any symbols that need to be resolved
   at this step. If a symbol should be relocated, it should be added to the
   relocation table (RELTBL), and the fields for that symbol should be set to
   all zeros. 

   You must perform error checking on all instructions and make sure that their
   arguments are valid. If an instruction is invalid, you should not write 
   anything to OUTPUT but simply return -1. MARS may be a useful resource for
   this step.

   Note the use of helper functions. Consider writing your own! If the function
   definition comes afterwards, you must declare it first (see translate.h).

   Returns 0 on success and -1 on error. 
 */
int translate_inst(FILE* output, const char* name, char** args, size_t num_args, uint32_t addr, SymbolTable* symtbl, SymbolTable* reltbl) {
	if (strcmp(name, "addu") == 0)       return write_rtype (0x21, output, args, num_args); /* `addiu` */
	else if (strcmp(name, "or") == 0)    return write_rtype (0x25, output, args, num_args); /* `or` */
	else if (strcmp(name, "sll") == 0)   return write_shift (0x00, output, args, num_args); /* `sll` */
	else if (strcmp(name, "slt") == 0)   return write_rtype (0x2a, output, args, num_args); /* `slt` */
	else if (strcmp(name, "sltu") == 0)  return write_rtype (0x2b, output, args, num_args); /* `sltu` */
	else if (strcmp(name, "jr") == 0)    return write_jr    (0x08, output, args, num_args); /* `jr` */
	else if (strcmp(name, "addiu") == 0) return write_addiu (0x09, output, args, num_args); /* `addiu` */
	else if (strcmp(name, "ori") == 0)   return write_ori   (0x0d, output, args, num_args); /* `ori` */
	else if (strcmp(name, "lui") == 0)   return write_lui   (0x0f, output, args, num_args); /* `lui` */
	else if (strcmp(name, "lb") == 0)    return write_mem   (0x20, output, args, num_args); /* `lb` */
	else if (strcmp(name, "lbu") == 0)   return write_mem   (0x24, output, args, num_args); /* `lbu` */
	else if (strcmp(name, "lw") == 0)    return write_mem   (0x23, output, args, num_args); /* `lw` */
	else if (strcmp(name, "sb") == 0)    return write_mem   (0x28, output, args, num_args); /* `sb` */
	else if (strcmp(name, "sw") == 0)    return write_mem   (0x2b, output, args, num_args); /* `sw` */
	else if (strcmp(name, "beq") == 0)   return write_branch(0x04, output, args, num_args, addr, symtbl); /* `beq` */
	else if (strcmp(name, "bne") == 0)   return write_branch(0x05, output, args, num_args, addr, symtbl); /* `bne` */
	else if (strcmp(name, "j") == 0)     return write_jump  (0x02, output, args, num_args, addr, reltbl); /* `j` */
	else if (strcmp(name, "jal") == 0)   return write_jump  (0x03, output, args, num_args, addr, reltbl); /* `jal` */
	else                                 return -1; /* Error */
}

/* A helper function for writing most R-type instructions. You should use
   translate_reg() to parse registers and write_inst_hex() to write to 
   OUTPUT. Both are defined in translate_utils.h.

   This function is INCOMPLETE. Complete the implementation below. You will
   find bitwise operations to be the cleanest way to complete this function.
 */
int write_rtype(uint8_t funct, FILE* output, char** args, size_t num_args) {
  /* DECLARATIONS */
	int rd, rs, rt;
	uint32_t instruction;
	if (num_args != 3) return -1; /* Basic error checking */ 
  /* Assign registers and immdiates */
	rd = translate_reg(args[0]);
	rs = translate_reg(args[1]);
	rt = translate_reg(args[2]);
  /* Error checking for assignments */
	if (rd == -1 || rs == -1 || rt == -1) return -1;
  /* Generate instruction */
	instruction = 0 | (rs<<21) | (rt<<16) | (rd<<11) | funct;
	write_inst_hex(output, instruction);
	return 0;
}

/* A helper function for writing shift instructions. You should use 
   translate_num() to parse numerical arguments. translate_num() is defined
   in translate_utils.h.

   This function is INCOMPLETE. Complete the implementation below. You will
   find bitwise operations to be the cleanest way to complete this function.
 */
int write_shift(uint8_t funct, FILE* output, char** args, size_t num_args) {
  /* DECLARATIONS */
	int rd, rt, err;
	long int shamt;
	uint32_t instruction;
	if (num_args != 3) return -1; /* Basic error checking */
  /* Assign registers and immdiates */
	rd = translate_reg(args[0]);
	rt = translate_reg(args[1]);
	err = translate_num(&shamt, args[2], 31, 0);
  /* Error checking for assignments */
	if (rd == -1 || rt == -1 || err == -1) return -1;
  /* Generate instruction */
	instruction = 0 | (rt<<16) | (rd<<11) | (shamt<<6) | funct;
	write_inst_hex(output, instruction);
	return 0;
}


int write_jr(uint8_t funct, FILE* output, char** args, size_t num_args) {
  /* DECLARATIONS */
	int rs; 
	uint32_t instruction;
	if (num_args != 1) return -1; /* Basic error checking */
  /* Assign registers and immdiates */
	rs = translate_reg(args[0]);
  /* Error checking for assignments */
	if (rs == -1) return -1;
  /* Generate instruction */
	instruction = 0 | (rs<<21) | funct;
	write_inst_hex(output, instruction);
	return 0;
}

int write_addiu(uint8_t opcode, FILE* output, char** args, size_t num_args) {
  /* DECLARATIONS */
	int rt, rs, err;
	long int imm;
	uint32_t instruction;
	if (num_args != 3) return -1; /* Basic error checking */
  /* Assign registers and immdiates */
	rt = translate_reg(args[0]);
	rs = translate_reg(args[1]);
	err = translate_num(&imm, args[2], 32767, -32768);
  /* Error checking for assignments */
	if (rt == -1 || rs == -1 || err == -1) return -1;
  /* Generate instruction */
	instruction = 0 | (opcode<<26) | (rs<<21) | (rt<<16) | (imm & 0xffff);
	write_inst_hex(output, instruction);
	return 0;
}

int write_ori(uint8_t opcode, FILE* output, char** args, size_t num_args) {
  /* DECLARATIONS */
	int rt, rs, err;
	long int imm;
	uint32_t instruction;
	if (num_args != 3) return -1; /* Basic error checking */
  /* Assign registers and immdiates */
	rt = translate_reg(args[0]);
	rs = translate_reg(args[1]);
	err = translate_num(&imm, args[2], 65535, 0);
  /* Error checking for assignments */
	if (rt == -1 || rs == -1 || err == -1) return -1;
  /* Generate instruction */
	instruction = 0 | (opcode<<26) | (rs<<21) | (rt<<16) | (imm & 0xffff);
	write_inst_hex(output, instruction);
	return 0;
}

int write_lui(uint8_t opcode, FILE* output, char** args, size_t num_args) {
  /* DECLARATIONS */
	int rt, err;
	long int imm;
	uint32_t instruction;
	if (num_args != 2) return -1; /* Basic error checking */
  /* Assign registers and immdiates */
	rt = translate_reg(args[0]);
	err = translate_num(&imm, args[1], 65535, 0);
  /* Error checking for assignments */
	if (rt == -1 || err == -1) return -1;
  /* Generate instruction */
	instruction = 0 | (opcode<<26) | (rt<<16) | (imm & 0xffff);
	write_inst_hex(output, instruction);
	return 0;
}


int write_mem(uint8_t opcode, FILE* output, char** args, size_t num_args) {
  /* DECLARATIONS */
	int rt, err, rs;
	long int offset;
	uint32_t instruction;
	if (num_args != 3) return -1; /* Basic error checking */
  /* Assign registers and immdiates */
	rt = translate_reg(args[0]);
	err = translate_num(&offset, args[1], 32767, -32768);
	rs = translate_reg(args[2]);
  /* Error checking for assignments */
	if (rt == -1 || rs == -1 || err == -1) return -1;
  /* Generate instruction */
	instruction = 0 | (opcode<<26) | (rs<<21) | (rt<<16) | (offset & 0xffff);
	write_inst_hex(output, instruction);
	return 0;
}

/* Hint: the way for branch to calculate relative address. e.g. bne
	 bne $rs $rt label
   assume the byte_offset(addr) of label is L, 
   current instruction byte_offset(addr) is A
   the relative address I  for label satisfy:
	 L = (A + 4) + I * 4
   so the relative addres is 
	 I = (L - A - 4) / 4;  */
int write_branch(uint8_t opcode, FILE* output, char** args, size_t num_args, 
		 uint32_t addr, SymbolTable* symtbl) {
  /* DECLARATIONS */
	int rs, rt;
	char* label;
	uint32_t instruction;
	int64_t label_addr, imm_addr; /* Addresses */
	if (num_args != 3) return -1; /* Basic error checking */
  /* Assign registers and labels */
	rs = translate_reg(args[0]);
	rt = translate_reg(args[1]);
	label = args[2];
  /* Get label address and handle errors */
	if (rt == -1 || rs == -1 || !is_valid_label(label)) return -1;
	label_addr = get_addr_for_symbol(symtbl, label);
	if (label_addr == -1) return -1;
	imm_addr = (label_addr - addr - 4) / 4; /* Translate to relative addr I */
	if (!((-32768 <= -imm_addr) && (imm_addr <= 32767))) return -1; /* Treat large relative address as error */
  /* Generate instruction */
	instruction = 0 | (opcode<<26) | (rs<<21) | (rt<<16) | (imm_addr & 0xffff);
	write_inst_hex(output, instruction);
	return 0;
}

/* Hint: the relocation table should record
   1. the current instruction byte_offset(addr)
   2. the unsolved LABEL in the jump instruction  */
int write_jump(uint8_t opcode, FILE* output, char** args, size_t num_args, 
		   uint32_t addr, SymbolTable* reltbl) {
  /* DECLARATIONS */
	int err;
	char* label;
	uint32_t instruction;
	if (num_args != 1) return -1; /* Basic error checking */
  /* Assign labels */
	label = args[0];
  /* Add to reltbl and handle error */
	if (!is_valid_label(label)) return -1;
	err = add_to_table(reltbl, label, addr);
	if (err == -1) return -1;
  /* Generate instruction */
	instruction = 0 | (opcode<<26);
	write_inst_hex(output, instruction);
	return 0;
}
