#include <stdint.h>
#include <stdio.h>
#include <conio.h>

#include "logo_parser.h"

#define FILENAME  "logo.txt"

logoInstructionDef_t* logo;

boolean create_logo_file(uint8_t type, char* filename);

int main(int argc, char* argv[])
{
	char* source_filename = FILENAME;
	uint16_t count = 0;

	create_logo_file(1, "logodefs.h");
//	create_logo_file(2, "logohash.txt");
	create_logo_file(3, "logocode.h");

	if (argc > 1) {
		source_filename = argv[1];
	}
	logo = logo_compile(&count, source_filename);
	if (argc > 2) {
		if (!logo_save_hex(logo, count, argv[2])) {
			printf("ERROR: failed to save logo %s\r\n", argv[2]);
		}

	}
	printf("Hit any key to exit\r\n");
	while (!_kbhit());
	return 0;
}
