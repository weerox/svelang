#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "lexer.h"
#include "token.h"

struct ast_node *parse_program(struct lexer *lexer);
struct ast_node *parse_expression(struct lexer *lexer);
struct ast_node *parse_term(struct lexer *lexer);
struct ast_node *parse_factor(struct lexer *lexer);
struct ast_node *parse_number(struct lexer *lexer);

struct ast_node *parse(struct lexer *lexer) {
	return parse_program(lexer);
}

struct ast_node *parse_program(struct lexer *lexer) {
	struct token token = lexer_peek(lexer);

	struct ast_node *ast = ast_statements_new();

	while (token.type != EOP) {
		if (token.type == EOL) {
			lexer_consume(lexer);
			token = lexer_peek(lexer);
			continue;
		}

		struct ast_node *statement = parse_expression(lexer);

		if (statement == NULL)
			return NULL;

		ast_statements_add(ast, statement);

		token = lexer_peek(lexer);
	}

	return ast;
}

struct ast_node *parse_expression(struct lexer *lexer) {
	struct ast_node *ast = parse_term(lexer);

	if (ast == NULL)
		return NULL;

	struct token operation = lexer_peek(lexer);

	while (operation.type == PLUS || operation.type == MINUS) {
		lexer_consume(lexer);

		struct ast_node *right = parse_term(lexer);

		if (right == NULL)
			return NULL;

		enum ast_operation op;

		if (operation.type == PLUS)
			op = AST_ADD;
		else if (operation.type == MINUS)
			op = AST_SUBTRACT;

		ast = ast_binary_new(ast, right, op);

		operation = lexer_peek(lexer);
	}

	return ast;
}

struct ast_node *parse_term(struct lexer *lexer) {
	struct ast_node *ast = parse_factor(lexer);

	if (ast == NULL)
		return NULL;

	struct token operation = lexer_peek(lexer);

	while (operation.type == MULTIPLICATION) {
		lexer_consume(lexer);
		struct ast_node *right = parse_factor(lexer);

		if (right == NULL)
			return NULL;

		ast = ast_binary_new(ast, right, AST_MULTIPLY);

		operation = lexer_peek(lexer);
	}

	return ast;
}

struct ast_node *parse_factor(struct lexer *lexer) {
	struct ast_node *ast = parse_number(lexer);

	return ast;
}

struct ast_node *parse_number(struct lexer *lexer) {
	struct token num = lexer_next(lexer);

	if (num.type != NUMBER) {
		fprintf(stderr, "%i:%i ", num.line, num.column);

		if (num.type == EOL)
			fprintf(stderr, "unexpected end of line");
		else if (num.type == EOP)
			fprintf(stderr, "unexpected end of program");
		else
			fprintf(stderr, "unexpected token '%s'", num.value);

		fprintf(stderr, "\n");

		return NULL;
	}

	struct ast_node *ast = ast_number_new(atoi(num.value));

	return ast;
}
