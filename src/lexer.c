#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "token.h"

#define SPACE 32
#define NEWLINE 10

struct lexer *lexer_new(char *file) {
	FILE *fp = fopen(file, "r");

	if (fp == NULL)
		return NULL;

	struct lexer *lexer = malloc(sizeof(struct lexer));
	lexer->fp = fp;

	lexer->line = 1;
	lexer->column = 1;

	return lexer;
}

struct token lexer_next(struct lexer *lexer) {
	struct token token;

	token.line = lexer->line;
	token.column = lexer->column;

	char *t = malloc(1);
	int t_len = 0;
	char c;

	*t = 0;

	int newline = 0;

	while (!feof(lexer->fp)) {
		fread(&c, 1, 1, lexer->fp);
		lexer->column++;

		if (c == SPACE) {
			if (t_len != 0) {
				break;
			} else {
				token.column++;
				continue;
			}
		}

		if (c == NEWLINE) {
			if (t_len != 0) {
				// move file pointer back one step so the newline can be read again
				// on the next token
				fseek(lexer->fp, -1, SEEK_CUR);
				lexer->column--;
			} else {
				newline = 1;
				lexer->line++;
				lexer->column = 1;
			}

			break;
		}

		char *t_n = malloc(t_len + 1 + 1);
		strcpy(t_n, t);
		*(t_n + t_len) = c;
		t_len++;
		*(t_n + t_len) = 0;

		free(t);

		t = t_n;
	}

	int number = 1;
	char *t_c = t;

	while (*t_c) {
		if (!isdigit(*t_c)) {
			number = 0;
			break;
		}

		t_c++;
	}

	if (newline) {
		token.type = EOL;
	} else if (feof(lexer->fp)) {
		token.type = EOP;
	} else if (number) {
		token.type = NUMBER;
		token.value = t;
	} else if (strcmp(t, "plus") == 0) {
		token.type = PLUS;
	} else if (strcmp(t, "minus") == 0) {
		token.type = MINUS;
	} else if (strcmp(t, "gånger") == 0) {
		token.type = MULTIPLICATION;
	} else {
		token.type = UNKNOWN;
		token.value = t;
	}

	return token;
}

void lexer_free(struct lexer *lexer) {
	fclose(lexer->fp);
	free(lexer);
}
