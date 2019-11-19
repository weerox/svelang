#include <stdio.h>

#include <llvm-c/Core.h>

#include "ast.h"
#include "lexer.h"
#include "parser.h"
#include "symbol.h"
#include "token.h"
#include "visitor.h"

LLVMValueRef visit(struct ast_node *node) {
	return node->visit(node);
}

LLVMValueRef visit_number(struct ast_node *node) {
	return LLVMConstInt(LLVMInt16Type(), node->number, 1);
}

LLVMValueRef visit_variable(struct ast_node *node) {
	struct symbol *sym = symbol_table_get(st, node->variable);

	/* symbol_table_get returns NULL if the variable */
	/* hasn't been declared */
	if (sym == NULL) {
		fprintf(stderr,
			"the variable '%s' hasn't been declared\n", node->variable
		);
		return NULL;
	}

	return LLVMBuildLoad(builder, sym->value, "var");
}

LLVMValueRef visit_binary(struct ast_node *node) {
	LLVMValueRef left = visit(node->binary.left);
	LLVMValueRef right = visit(node->binary.right);

	if (!left || !right)
		return NULL;

	switch (node->binary.op) {
		case AST_ADD:
			return LLVMBuildAdd(builder, left, right, "add");
		case AST_SUBTRACT:
			return LLVMBuildSub(builder, left, right, "sub");
		case AST_MULTIPLY:
			return LLVMBuildMul(builder, left, right, "mul");
	}
}

LLVMValueRef visit_statements(struct ast_node *node) {
	for (unsigned int i = 0; i < node->statements.len; i++) {
		visit(*(node->statements.nodes + i));
	}

	return NULL;
}

LLVMValueRef visit_initialize(struct ast_node *node) {
	/* evaluate the expression before creating a variable */
	/* in the symbol table, so that interpreter_variable will */
	/* return an error if the variable to be initialized */
	/* is used in the expression */
	LLVMValueRef value = visit(node->initialize.expression);

	if (!value)
		return NULL;

	char *name = node->initialize.variable->variable;

	/* check that the variable hasn't already been declared */
	if (symbol_table_get(st, name) != NULL) {
		fprintf(stderr,
			"the variable '%s' has already been declared\n",
			node->initialize.variable->variable
		);
		return NULL;
	}

	struct symbol *sym = symbol_table_put(st, name);

	if (sym == NULL)
		return NULL;

	LLVMValueRef var = LLVMBuildAlloca(builder, LLVMInt16Type(), "var");

	LLVMBuildStore(builder, value, var);

	sym->value = var;

	return NULL;
}

LLVMValueRef visit_write_newline(struct ast_node *node) {
	LLVMBuildCall(builder, pf, &pf_nl, 1, "printf_nl");

	return NULL;
}

LLVMValueRef visit_write(struct ast_node *node) {
	LLVMValueRef args[2];

	args[0] = pf_i;
	args[1] = visit(node->write.expression);

	LLVMBuildCall(builder, pf, args, 2, "printf_i");

	return NULL;
}
