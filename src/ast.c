#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "lexer.h"

struct ast_node *ast_number_new(int number) {
	struct ast_node *ast = malloc(sizeof(struct ast_node));

	ast->type = AST_NUMBER;
	ast->number = number;

	return ast;
}

struct ast_node *ast_variable_new(char *variable) {
	struct ast_node *ast = malloc(sizeof(struct ast_node));

	ast->type = AST_VARIABLE;
	ast->variable = variable;

	return ast;
}

struct ast_node *ast_binary_new(
	struct ast_node *left, struct ast_node *right, enum ast_operation op
) {
	struct ast_node *ast = malloc(sizeof(struct ast_node));

	ast->type = AST_BINARY;
	ast->binary.left = left;
	ast->binary.right = right;
	ast->binary.op = op;

	return ast;
}

struct ast_node *ast_statements_new() {
	struct ast_node *ast = malloc(sizeof(struct ast_node));

	ast->type = AST_STATEMENTS;
	ast->statements.len = 0;
	ast->statements.nodes = NULL;

	return ast;
}

struct ast_node *ast_statements_add(
	struct ast_node *ast, struct ast_node *node
) {
	ast->statements.nodes = realloc(
		ast->statements.nodes,
		sizeof(struct ast_node *) * (ast->statements.len + 1)
	);
	*(ast->statements.nodes + ast->statements.len) = node;
	ast->statements.len += 1;

	return ast;
}

struct ast_node *ast_initialize_new(
	struct ast_node *variable, struct ast_node *expression
) {
	struct ast_node *ast = malloc(sizeof(struct ast_node));

	ast->type = AST_INITIALIZE;
	ast->initialize.variable = variable;
	ast->initialize.expression = expression;

	return ast;
}

void ast_print_indent(unsigned int level) {
	for (int i = 0; i < level; i++) {
		printf("  ");
	}
}

void ast_print_level(struct ast_node *ast, unsigned int level) {
	switch (ast->type) {
		case AST_NUMBER:
			ast_print_indent(level);
			printf("num: %i\n", ast->number);
			break;
		case AST_VARIABLE:
			ast_print_indent(level);
			printf("var: %s\n", ast->variable);
			break;
		case AST_BINARY:
			ast_print_indent(level);
			printf("bin: ");
			switch (ast->binary.op) {
				case AST_ADD:
					printf("+");
					break;
				case AST_SUBTRACT:
					printf("-");
					break;
				case AST_MULTIPLY:
					printf("*");
					break;
			}
			printf("\n");
			ast_print_indent(level + 1);
			printf("left:\n");
			ast_print_level(ast->binary.left, level + 2);

			ast_print_indent(level + 1);
			printf("right:\n");
			ast_print_level(ast->binary.right, level + 2);
			break;
		case AST_STATEMENTS:
			ast_print_indent(level);
			printf("stm: %i\n", ast->statements.len);

			for (int i = 0; i < ast->statements.len; i++) {
				ast_print_level(*(ast->statements.nodes + i), level + 1);
			}
			break;
		case AST_INITIALIZE:
			ast_print_indent(level);
			printf("init:\n");
			ast_print_level(ast->initialize.variable, level + 1);
			ast_print_level(ast->initialize.expression, level + 1);
			break;
	}
}

void ast_print(struct ast_node *ast) {
	ast_print_level(ast, 0);
}
