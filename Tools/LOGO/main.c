#include <stdint.h>
#include <stdio.h>
#include <conio.h>

#include "logo_parser.h"

#define FILENAME  "logo.txt"
#define MAX_OPCODES 200
logoInstructionDef_t logo[MAX_OPCODES];

boolean create_logo_file(uint8_t type, char* filename);

int main(int argc, char* argv[])
{
	// TODO: set this up to support tradional stdin/stdout redirection
	char* source_filename = FILENAME;

	create_logo_file(1, "logodefs.h");
//	create_logo_file(2, "logohash.txt");
	create_logo_file(3, "logocode.h");

	if (argc > 1) {
		source_filename = argv[1];
	}

	if (argc > 2) {
		if (!logo_compile(source_filename, argv[2])) {
			printf("ERROR: failed to compile logo script %s\r\n", argv[2]);
		}
	} else {
		if (!logo_compose(&logo[0], sizeof(logo)/sizeof(logo[0]), source_filename)) {
			printf("ERROR: failed to parse logo script %s\r\n", source_filename);
		}
	}

	printf("Hit any key to exit\r\n");
	while (!_kbhit());
	return 0;
}
