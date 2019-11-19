#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "lexer.h"
#include "token.h"

struct ast_node *parse_program(struct lexer *lexer);
struct ast_node *parse_initializer(struct lexer *lexer);
struct ast_node *parse_expression(struct lexer *lexer);
struct ast_node *parse_term(struct lexer *lexer);
struct ast_node *parse_factor(struct lexer *lexer);
struct ast_node *parse_number(struct lexer *lexer);
struct ast_node *parse_variable(struct lexer *lexer);
struct ast_node *parse_write(struct lexer *lexer);

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

		struct ast_node *statement = NULL;

		if (token.type == LET)
			statement = parse_initializer(lexer);
		else if (token.type == WRITE)
			statement = parse_write(lexer);
		else
			statement = parse_expression(lexer);

		if (statement == NULL)
			return NULL;

		ast_statements_add(ast, statement);

		token = lexer_peek(lexer);
	}

	return ast;
}

struct ast_node *parse_initializer(struct lexer *lexer) {
	if (lexer_peek(lexer).type != LET)
		return NULL;

	lexer_consume(lexer);

	struct ast_node *var = parse_variable(lexer);

	if (var == NULL)
		return NULL;

	if (lexer_peek(lexer).type != BE)
		return NULL;

	lexer_consume(lexer);

	struct ast_node *expr = parse_expression(lexer);

	if (expr == NULL)
		return NULL;

	struct ast_node *ast = ast_initialize_new(var, expr);

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
	struct token t = lexer_peek(lexer);

	struct ast_node *ast = NULL;

	if (t.type == NUMBER)
		ast = parse_number(lexer);
	else
		ast = parse_variable(lexer);

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

struct ast_node *parse_variable(struct lexer *lexer) {
	struct token var = lexer_next(lexer);

	// can't use keywords as variable name
	if (var.type != UNKNOWN) {
		fprintf(stderr, "%i:%i '%s' ", var.line, var.column, var.value);
		fprintf(stderr, "is a registered keyword and ");
		fprintf(stderr, "cannot be used as a variable name\n");

		return NULL;
	}

	struct ast_node *ast = ast_variable_new(var.value);

	return ast;
}

struct ast_node *parse_write(struct lexer *lexer) {
	if (lexer_peek(lexer).type != WRITE)
		return NULL;

	lexer_consume(lexer);

	if (lexer_peek(lexer).type == NEW) {
		lexer_consume(lexer);
		if (lexer_peek(lexer).type == LINE) {
			lexer_consume(lexer);

			struct ast_node *ast = ast_write_newline_new();
			return ast;
		} else {
			fprintf(stderr, "expected line\n");
			return NULL;
		}
	} else {
		struct ast_node *expression = parse_expression(lexer);
		struct ast_node *ast = ast_write_new(expression);
		return ast;
	}
	return NULL;
}
