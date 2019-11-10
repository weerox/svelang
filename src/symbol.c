#include <stdlib.h>
#include <string.h>

#include "symbol.h"

#define INITIAL_CAPACITY 1 << 4

struct symbol_table *symbol_table_new() {
	struct symbol_table *st = malloc(sizeof(struct symbol_table));

	if (st == NULL)
		return NULL;

	st->capacity = INITIAL_CAPACITY;
	st->size = 0;

	st->nodes = calloc(st->capacity, sizeof(struct symbol_table_node *));

	if (st->nodes == NULL)
		return NULL;

	return st;
}

unsigned int symbol_hash(char *name) {
	/* hash the symbol name using djb2 */
	/* http://www.cse.yorku.ca/~oz/hash.html */

	unsigned int hash = 5381;
	int c;

	while ((c = *name++) != 0)
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

	return hash;
}

struct symbol *symbol_table_get(struct symbol_table *st, char *name) {
	unsigned int index = symbol_hash(name) % st->capacity;

	struct symbol *sym = NULL;
	struct symbol_table_node *stn = *(st->nodes + index);

	while (stn != NULL) {
		if (strcmp(stn->symbol->name, name) == 0)
			sym = stn->symbol;
			break;
		stn = stn->next;
	}

	return sym;
}

struct symbol *symbol_table_put(struct symbol_table *st, char *name) {
	unsigned int index = symbol_hash(name) % st->capacity;

	struct symbol *sym = malloc(sizeof(struct symbol));

	if (sym == NULL)
		return NULL;

	sym->name = malloc(strlen(name) + 1);
	strcpy(sym->name, name);

	sym->value = NULL;

	struct symbol_table_node *stn = malloc(sizeof(struct symbol_table_node));

	if (stn == NULL)
		return NULL;

	stn->symbol = sym;
	stn->next = NULL;

	if (*(st->nodes + index) == NULL) {
		*(st->nodes + index) = stn;
	} else {
		struct symbol_table_node *n = *(st->nodes + index);

		while (n->next != NULL)
			n = n->next;

		n->next = stn;
	}

	return sym;
}
