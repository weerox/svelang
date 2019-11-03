#ifndef SVELANG_TOKEN_H
#define SVELANG_TOKEN_H

enum token_type {
	EndOfLine,
	EndOfProgram,
	Unknown,
	Number,
	Multiplication,
	Plus,
	Minus
};

struct token {
	enum token_type type;
	char *value;
	int line;
	int column;
};

#endif /* SVELANG_TOKEN_H */
