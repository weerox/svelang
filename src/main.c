#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <llvm-c/Analysis.h>
#include <llvm-c/BitWriter.h>
#include <llvm-c/Core.h>

#include "ast.h"
#include "lexer.h"
#include "parser.h"
#include "symbol.h"
#include "visitor.h"

void usage() {
	fprintf(stderr, "usage: svl <file>\n");
	fprintf(stderr, "\n");
}

int main(int argc, char **argv) {
	char c;

	// getopt shouldn't print errors
	opterr = 0;

	while ((c = getopt(argc, argv, "h")) != -1) {
		switch (c) {
			case 'h':
				usage();
				return 0;
			default:
				usage();
				return 1;
		}
	}

	if (argc - optind != 1) {
		usage();
		return 1;
	}

	char *file = malloc(strlen(*(argv + optind)) + 1);
	strcpy(file, *(argv + optind));

	if (access(file, R_OK) != 0) {
		// the file provided by the user couldn't be read
		// check if the provided file had the right file extension

		if (strcmp(file + strlen(file) - 4, ".svl") != 0) {
			char *file_e = malloc(strlen(file) + 4 + 1);
			strcpy(file_e, file);
			strcat(file_e, ".svl");

			if (access(file_e, R_OK) == 0) {
				file = file_e;
			} else {
				usage();
				free(file);
				free(file_e);
				return 1;
			}
		} else {
			usage();
			free(file);
			return 1;
		}
	}

	struct lexer *lexer = lexer_new(file);

	if (lexer == NULL) {
		fprintf(stderr, "The lexer couldn't be created\n");
		return 1;
	}

	struct ast_node *ast = parse(lexer);

	if (ast == NULL) {
		return 1;
	}

	lexer_free(lexer);

	LLVMModuleRef module = LLVMModuleCreateWithName("svelang");

	LLVMTypeRef main_t = LLVMFunctionType(LLVMVoidType(), NULL, 0, 0);
	LLVMValueRef main_f = LLVMAddFunction(module, "main", main_t);

	LLVMBasicBlockRef main_b = LLVMAppendBasicBlock(main_f, "main");

	builder = LLVMCreateBuilder();
	LLVMPositionBuilderAtEnd(builder, main_b);

	st = symbol_table_new();

	visit(ast);

	LLVMBuildRetVoid(builder);

	char *error = NULL;
	LLVMVerifyModule(module, LLVMAbortProcessAction, &error);
	LLVMDisposeMessage(error);

	char *base = malloc(strlen(file) - 4 + 1);
	strncpy(base, file, strlen(file) - 4);

	char *file_bc = malloc(strlen(base) + 3 + 1);
	strcpy(file_bc, base);
	strcat(file_bc, ".bc");

	if (LLVMWriteBitcodeToFile(module, file_bc) != 0) {
		fprintf(stderr, "couldn't write bitcode to file\n");
	}

	char *bc_2_obj = malloc(18 + strlen(file_bc) + 1);
	strcpy(bc_2_obj, "llc -filetype=obj ");
	strcat(bc_2_obj, file_bc);

	if (system(bc_2_obj) != 0) {
		fprintf(stderr, "couldn't create object file\n");
		return 1;
	}

	char *comp = malloc(4 + strlen(base) + 2 + 1);
	strcpy(comp, "gcc ");
	strcat(comp, base);
	strcat(comp, ".o");

	if (system(comp) != 0) {
		fprintf(stderr, "couldn't compile object file\n");
		return 1;
	}

	printf("program saved as 'a.out'\n");

	LLVMDisposeModule(module);

	return 0;
}
