#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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

	return 0;
}
