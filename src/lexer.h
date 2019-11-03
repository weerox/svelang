#ifndef SVELANG_LEXER_H
#define SVELANG_LEXER_H

#include <stdio.h>

#include "token.h"

struct lexer {
	FILE *fp;
	int line;
	int column;
};

struct lexer *lexer_new(char *file);
struct token lexer_next(struct lexer *lexer);
void lexer_free(struct lexer *lexer);

#endif /* SVELANG_LEXER_H */
