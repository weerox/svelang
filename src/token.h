#ifndef SVELANG_TOKEN_H
#define SVELANG_TOKEN_H

enum token_type {
	EOL, // End of line
	EOP, // End of program
	UNKNOWN,
	NUMBER,
	MULTIPLICATION,
	PLUS,
	MINUS,
	BE,
	LET,
	WRITE,
	NEW,
	LINE
};

struct token {
	enum token_type type;
	char *value;
	int line;
	int column;
};

#endif /* SVELANG_TOKEN_H */
