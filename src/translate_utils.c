#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "translate_utils.h"

void write_inst_string(FILE* output, const char* name, char** args, int num_args) {
  int i;

  fprintf(output, "%s", name);
  for (i = 0; i < num_args; i++) {
	fprintf(output, " %s", args[i]);
  }
  fprintf(output, "\n");
}

void write_inst_hex(FILE *output, uint32_t instruction) {
	fprintf(output, "%08x\n", instruction);
}

int is_valid_label(const char* str) {
	int first = 1;

	if (!str) {
		return 0;
	}

	while (*str) {
		if (first) {
			if (!isalpha((int) *str) && *str != '_') {
		  return 0;   /* does not start with letter or underscore */
			} else {
				first = 0;
			}
		} else if (!isalnum((int) *str) && *str != '_') {
	  return 0;       /* subsequent characters not alphanumeric */
		}
		str++;
	}
	return first ? 0 : 1;   /* empty string is invalid  */
}

/* Translate the input string into a signed number. The number is then 
   checked to be within the correct range (note bounds are INCLUSIVE)
   ie. NUM is valid if LOWER_BOUND <= NUM <= UPPER_BOUND. 

   The input may be in either positive or negative, and be in either
   decimal or hexadecimal format. It is also possible that the input is not
   a valid number. Fortunately, the library function strtol() can take 
   care of all that (with a little bit of work from your side of course).
   Please read the documentation for strtol() carefully. Do not use strtoul()
   or any other variants. 

   You should store the result into the location that OUTPUT points to. The 
   function returns 0 if the conversion proceeded without errors, or -1 if an 
   error occurred.
 */
int translate_num(long int* output, const char* str, long int upper_bound, 
				  long int lower_bound) {
  /* DECLARATIONS */
	char* endptr; /* For strtol checking */
	long int num;
	if (!str || !output) return -1; /* Basic error checking */
  /* Translate to long int */
	num = strtol(str, &endptr, 0);
	if (endptr != str + strlen(str)) return -1; /* Not a number */
	if ((lower_bound <= num) && (num <= upper_bound)) { /* Success */
		*output = num;
		return 0;
	} else return -1; /* Over range */
}

/* Translates the register name to the corresponding register number. Please
   see the MIPS Green Sheet for information about register numbers.

   Returns the register number of STR or -1 if the register name is invalid.
 */
int translate_reg(const char* str) {
	if (!str) return -1; /* Basic error checking */
	if (strcmp(str, "$zero") == 0)      return 0;  /* $zero */
	else if (strcmp(str, "$0") == 0)    return 0;  /* lieu for $zero */
	else if (strcmp(str, "$at") == 0)   return 1;  /* $at */
	else if (strcmp(str, "$v0") == 0)   return 2;  /* $v0 */
	else if (strcmp(str, "$v1") == 0)   return 3;  /* $v1 */
	else if (strcmp(str, "$a0") == 0)   return 4;  /* $a0 */
	else if (strcmp(str, "$a1") == 0)   return 5;  /* $a1 */
	else if (strcmp(str, "$a2") == 0)   return 6;  /* $a2 */
	else if (strcmp(str, "$a3") == 0)   return 7;  /* $a3 */
	else if (strcmp(str, "$t0") == 0)   return 8;  /* $t0 */
	else if (strcmp(str, "$t1") == 0)   return 9;  /* $t1 */
	else if (strcmp(str, "$t2") == 0)   return 10; /* $t2 */
	else if (strcmp(str, "$t3") == 0)   return 11; /* $t3 */
	else if (strcmp(str, "$t4") == 0)   return 12; /* $t4 */
	else if (strcmp(str, "$t5") == 0)   return 13; /* $t5 */
	else if (strcmp(str, "$t6") == 0)   return 14; /* $t6 */
	else if (strcmp(str, "$t7") == 0)   return 15; /* $t7 */
	else if (strcmp(str, "$s0") == 0)   return 16; /* $s0 */
	else if (strcmp(str, "$s1") == 0)   return 17; /* $s1 */
	else if (strcmp(str, "$s2") == 0)   return 18; /* $s2 */
	else if (strcmp(str, "$s3") == 0)   return 19; /* $s3 */
	else if (strcmp(str, "$s4") == 0)   return 20; /* $s4 */
	else if (strcmp(str, "$s5") == 0)   return 21; /* $s5 */
	else if (strcmp(str, "$s6") == 0)   return 22; /* $s6 */
	else if (strcmp(str, "$s7") == 0)   return 23; /* $s7 */
	else if (strcmp(str, "$t8") == 0)   return 24; /* $t8 */
	else if (strcmp(str, "$t9") == 0)   return 25; /* $t9 */
	else if (strcmp(str, "$k0") == 0)   return 26; /* $k0 */
	else if (strcmp(str, "$k1") == 0)   return 27; /* $k1 */
	else if (strcmp(str, "$gp") == 0)   return 28; /* $gp */
	else if (strcmp(str, "$sp") == 0)   return 29; /* $sp */
	else if (strcmp(str, "$fp") == 0)   return 30; /* $fp */
	else if (strcmp(str, "$ra") == 0)   return 31; /* $ra */
	else if (strcmp(str, "$1") == 0)    return 1;
	else if (strcmp(str, "$2") == 0)    return 2;
	else if (strcmp(str, "$3") == 0)    return 3;
	else if (strcmp(str, "$4") == 0)    return 4;
	else if (strcmp(str, "$5") == 0)    return 5;
	else if (strcmp(str, "$6") == 0)    return 6;
	else if (strcmp(str, "$7") == 0)    return 7;
	else if (strcmp(str, "$8") == 0)    return 8;
	else if (strcmp(str, "$9") == 0)    return 9;
	else if (strcmp(str, "$10") == 0)   return 10;
	else if (strcmp(str, "$11") == 0)   return 11;
	else if (strcmp(str, "$12") == 0)   return 12;
	else if (strcmp(str, "$13") == 0)   return 13;
	else if (strcmp(str, "$14") == 0)   return 14;
	else if (strcmp(str, "$15") == 0)   return 15;
	else if (strcmp(str, "$16") == 0)   return 16;
	else if (strcmp(str, "$17") == 0)   return 17;
	else if (strcmp(str, "$18") == 0)   return 18;
	else if (strcmp(str, "$19") == 0)   return 19;
	else if (strcmp(str, "$20") == 0)   return 20;
	else if (strcmp(str, "$21") == 0)   return 21;
	else if (strcmp(str, "$22") == 0)   return 22;
	else if (strcmp(str, "$23") == 0)   return 23;
	else if (strcmp(str, "$24") == 0)   return 24;
	else if (strcmp(str, "$25") == 0)   return 25;
	else if (strcmp(str, "$26") == 0)   return 26;
	else if (strcmp(str, "$27") == 0)   return 27;
	else if (strcmp(str, "$28") == 0)   return 28;
	else if (strcmp(str, "$29") == 0)   return 29;
	else if (strcmp(str, "$30") == 0)   return 30;
	else if (strcmp(str, "$31") == 0)   return 31;
	else                                return -1; /* Error */
}
