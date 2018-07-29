
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "utils.h"
#include "tables.h"

const int SYMBOLTBL_NON_UNIQUE = 0;
const int SYMBOLTBL_UNIQUE_NAME = 1;

/*******************************
 * Helper Functions
 *******************************/

void allocation_failed() {
	write_to_log("Error: allocation failed\n");
	exit(1);
}

void addr_alignment_incorrect() {
	write_to_log("Error: address is not a multiple of 4.\n");
}

void name_already_exists(const char* name) {
	write_to_log("Error: name '%s' already exists in table.\n", name);
}

void write_sym(FILE* output, uint32_t addr, const char* name) {
	fprintf(output, "%u\t%s\n", addr, name);
}

/*******************************
 * Symbol Table Functions
 *******************************/

/* Creates a new SymbolTable containg 0 elements and returns a pointer to that
   table. Multiple SymbolTables may exist at the same time. 
   If memory allocation fails, you should call allocation_failed(). 
   Mode will be either SYMBOLTBL_NON_UNIQUE or SYMBOLTBL_UNIQUE_NAME. You will need
   to store this value for use during add_to_table().
 */
SymbolTable* create_table(int mode) {
	SymbolTable* tbl = malloc(sizeof(SymbolTable)); /* Alloc for table */
	Symbol* head = malloc(sizeof(Symbol)); /* Alloc for header */
	if (!tbl) allocation_failed();
	if (!head) allocation_failed();
	head->name = NULL; /* Initialize header */
	head->addr = 0;
	head->next = NULL;
	tbl->head = head; /* Initialize table */
	tbl->tail = head;
	tbl->len = 0;
	tbl->mode = mode; /* Assign mode */
	return tbl;
}

/* Frees the given SymbolTable and all associated memory. */
void free_table(SymbolTable* table) {
	Symbol* del;
	while (table->head->next) { /* Loop for every non-header node */
		del = table->head->next;
		table->head->next = del->next;
		free(del->name); /* Delete the node */
		free(del);
	}
	free(table->head); /* Free header and table */
	free(table);
}

/* Adds a new symbol and its address to the SymbolTable pointed to by TABLE. 
   1. ADDR is given as the byte offset from the first instruction. 
   2. The SymbolTable must be able to resize itself as more elements are added. 

   3. Note that NAME may point to a temporary array, so it is not safe to simply
   store the NAME pointer. You must store a copy of the given string.

   4. If ADDR is not word-aligned, you should call addr_alignment_incorrect() 
   and return -1. 

   5. If the table's mode is SYMTBL_UNIQUE_NAME and NAME already exists 
   in the table, you should call name_already_exists() and return -1. 

   6.If memory allocation fails, you should call allocation_failed(). 

   Otherwise, you should store the symbol name and address and return 0.
 */
int add_to_table(SymbolTable* table, const char* name, uint32_t addr) {
  /* Check addr word alignment */
	if (addr % 4) {
		addr_alignment_incorrect();
		return -1;
	}
  /* Adding */
	if (table->mode == SYMBOLTBL_NON_UNIQUE) append_sym(table, name, addr); /* Non-unique mode, directly append to tail */
	else { /* Unique mode */
		Symbol* cur = table->head;
		while ((cur = cur->next)) {
			if (strcmp(cur->name, name) == 0) { /* If already exist, fail */
				name_already_exists(name);
				return -1;
			}
		}
		append_sym(table, name, addr); /* Else append to tail */
	}
	return 0;
}

/* Auxiliary function for appending a node to table tail */
void append_sym(SymbolTable* table, const char* name, uint32_t addr) {
	Symbol* sym = malloc(sizeof(Symbol)); /* Alloc for this node */
	if (!sym) allocation_failed();
	sym->name = malloc(strlen(name)+1); /* Copy name */
	if (!sym->name) allocation_failed();
	strcpy(sym->name, name);
	sym->addr = addr; /* Initialize the node */
	sym->next = NULL;
	table->tail->next = sym; /* Append the node to tail */
	table->tail = sym;
	table->len++;
}

/* Returns the address (byte offset) of the given symbol. If a symbol with name
   NAME is not present in TABLE, return -1.
 */
int64_t get_addr_for_symbol(SymbolTable* table, const char* name) {   
	Symbol* cur = table->head;
	while ((cur = cur->next)) if (strcmp(cur->name, name) == 0) return cur->addr; /* Loop through the list to search */
	return -1; /* Not found */
}

/* Writes the SymbolTable TABLE to OUTPUT. You should use write_sym() to
   perform the write. Do not print any additional whitespace or characters.
 */
void write_table(SymbolTable* table, FILE* output) {
	Symbol* cur = table->head;
	while ((cur = cur->next)) write_sym(output, cur->addr, cur->name); /* Loop through the list to write */
}
