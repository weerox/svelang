#ifndef SVELANG_SYMBOL_H
#define SVELANG_SYMBOL_H

#include <llvm-c/Core.h>

struct symbol {
	char *name;
	LLVMValueRef value;
};

struct symbol_table_node {
	struct symbol *symbol;
	struct symbol_table_node *next;
};

struct symbol_table {
	unsigned int capacity;
	unsigned int size;
	struct symbol_table_node **nodes;
};

struct symbol_table *symbol_table_new();
struct symbol *symbol_table_get();
struct symbol *symbol_table_put();

#endif /* SVELANG_SYMBOL_H */
