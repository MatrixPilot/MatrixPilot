
#if (WIN == 1 || NIX == 1)
#define DPRINT printf
#else
#define DPRINT(args, ...)
#endif // WIN

typedef int boolean;
#define false 0
#define true !false

typedef struct logoInstructionDef {
	uint16_t cmd        :  6;
	uint16_t do_fly     :  1;
	uint16_t use_param  :  1;
	uint16_t subcmd     :  8;
	int16_t arg         : 16;
} logoInstructionDef_t;

uint8_t logo_compose(logoInstructionDef_t opcode[], uint8_t size, char* filename);
uint8_t logo_compile(char* source_filename, char* logo_filename);
