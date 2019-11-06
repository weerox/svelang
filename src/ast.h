#ifndef SVELANG_AST_H
#define SVELANG_AST_H

#include "lexer.h"

enum ast_type {
	AST_BINARY,
	AST_NUMBER,
	AST_STATEMENTS
};

enum ast_operation {
	AST_ADD,
	AST_SUBTRACT,
	AST_MULTIPLY
};

struct ast_node {
	enum ast_type type;
	union {
		int number;
		struct {
			struct ast_node *left;
			struct ast_node *right;
			enum ast_operation op;
		} binary;
		struct {
			unsigned int len;
			struct ast_node **nodes;
		} statements;
	};
};

struct ast_node *ast_binary_new(
	struct ast_node *left, struct ast_node *right, enum ast_operation op
);
struct ast_node *ast_number_new(int number);
struct ast_node *ast_statements_new();
struct ast_node *ast_statements_add(
	struct ast_node *ast, struct ast_node *node
);

void ast_print(struct ast_node *ast);

#endif /* SVELANG_AST_H */
