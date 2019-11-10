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

	interpret_node(ast, NULL);

	return 0;
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
	*(int *) r = *(int *) sym->value;

printf("END interpret_variable\n");
	return 0;
}

int interpret_binary(struct ast_node *node, void *r) {
	if (r == NULL)
		return 0;

	int left, right;

	interpret_node(node->binary.left, &left);
	interpret_node(node->binary.right, &right);

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
	for (unsigned int i = 0; i < node->statements.len; i++)
		interpret_node(*(node->statements.nodes + i), r);

	return 0;
}

int interpret_initialize(struct ast_node *node, void *r) {
	char *name = node->initialize.variable->variable;

	struct symbol *sym = symbol_table_put(st, name);

	int *v = malloc(sizeof(int));

	interpret_node(node->initialize.expression, v);

	sym->value = v;

	return 0;
}
