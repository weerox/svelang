#ifndef SVELANG_VISITOR_H
#define SVELANG_VISITOR_H

#include <llvm-c/Core.h>

#include "ast.h"
#include "symbol.h"

LLVMValueRef visit(struct ast_node *node);
LLVMValueRef visit_number(struct ast_node *node);
LLVMValueRef visit_variable(struct ast_node *node);
LLVMValueRef visit_binary(struct ast_node *node);
LLVMValueRef visit_statements(struct ast_node *node);
LLVMValueRef visit_initialize(struct ast_node *node);
LLVMValueRef visit_write_newline(struct ast_node *node);
LLVMValueRef visit_write(struct ast_node *node);

struct symbol_table *st;

LLVMBuilderRef builder;

#endif /* SVELANG_VISITOR_H */
