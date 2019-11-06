#ifndef SVELANG_PARSER_H
#define SVELANG_PARSER_H

#include "ast.h"
#include "lexer.h"

struct ast_node *parse(struct lexer *lexer);

#endif /* SVELANG_PARSER_H */
