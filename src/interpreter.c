#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "lexer.h"
#include "parser.h"
#include "symbol.h"
#include "token.h"

int interpret_node(struct ast_node *node, void *r);
int interpret_number(struct ast_node *node, void *r);
int interpret_variable(struct ast_node *node, void *r);
int interpret_binary(struct ast_node *node, void *r);
int interpret_statements(struct ast_node *node, void *r);
int interpret_initialize(struct ast_node *node, void *r);

struct symbol_table *st;

int interpret(char *file) {
	struct lexer *lexer = lexer_new(file);

	if (lexer == NULL) {
		fprintf(stderr, "The lexer couldn't be created\n");
		return 1;
	}

	struct ast_node *ast = parse(lexer);

	if (ast == NULL) {
		return 1;
	}

	lexer_free(lexer);

	st = symbol_table_new();

	return interpret_node(ast, NULL);
}

int interpret_node(struct ast_node *node, void *r) {
	switch (node->type) {
		case AST_NUMBER:
			return interpret_number(node, r);
			break;
		case AST_VARIABLE:
			return interpret_variable(node, r);
			break;
		case AST_BINARY:
			return interpret_binary(node, r);
			break;
		case AST_STATEMENTS:
			return interpret_statements(node, r);
			break;
		case AST_INITIALIZE:
			return interpret_initialize(node, r);
			break;
	}
}

int interpret_number(struct ast_node *node, void *r) {
	*(int *) r = node->number;
	return 0;
}

int interpret_variable(struct ast_node *node, void *r) {
	struct symbol *sym = symbol_table_get(st, node->variable);

	/* symbol_table_get returns NULL if the variable */
	/* hasn't been declared */
	if (sym == NULL) {
		fprintf(stderr,
			"the variable '%s' hasn't been declared\n", node->variable
		);
		return 1;
	}

	*(int *) r = *(int *) sym->value;

printf("END interpret_variable\n");
	return 0;
}

int interpret_binary(struct ast_node *node, void *r) {
	/* the expression won't be used for anything */
	/* so there is no need to evaluate it */
	if (r == NULL)
		return 0;

	int left, right;
	int res = 0;

	res = interpret_node(node->binary.left, &left);

	if (res != 0)
		return res;

	res = interpret_node(node->binary.right, &right);

	if (res != 0)
		return res;

	switch (node->binary.op) {
		case AST_ADD:
			*(int *) r = left + right;
			break;
		case AST_SUBTRACT:
			*(int *) r = left - right;
			break;
		case AST_MULTIPLY:
			*(int *) r = left * right;
			break;
	}

	return 0;
}

int interpret_statements(struct ast_node *node, void *r) {
	for (unsigned int i = 0; i < node->statements.len; i++) {
		int res = interpret_node(*(node->statements.nodes + i), r);
		if (res != 0)
			return res;
	}

	return 0;
}

int interpret_initialize(struct ast_node *node, void *r) {
	/* evaluate the expression before creating a variable */
	/* in the symbol table, so that interpreter_variable will */
	/* return an error if the variable to be initialized */
	/* is used in the expression */
	int *v = malloc(sizeof(int));

	if (v == NULL)
		return 1;

	int res = interpret_node(node->initialize.expression, v);

	if (res != 0)
		return res;

	char *name = node->initialize.variable->variable;

	/* check that the variable hasn't already been declared */
	if (symbol_table_get(st, name) != NULL) {
		fprintf(stderr,
			"the variable '%s' has already been declared\n",
			node->initialize.variable->variable
		);
		return 1;
	}

	struct symbol *sym = symbol_table_put(st, name);

	if (sym == NULL)
		return 1;

	sym->value = v;

	return 0;
}
