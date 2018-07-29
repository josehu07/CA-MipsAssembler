#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#define MAX_ARGS 3
#define BUF_SIZE 1024

/*******************************
 * Do Not Modify Code Below
 *******************************/

int assemble(const char* in_name, const char* tmp_name, const char* out_name);

int pass_one(FILE *input, FILE* output, SymbolTable* symtbl);

int pass_two(FILE *input, FILE* output, SymbolTable* symtbl, SymbolTable* reltbl);

#endif
