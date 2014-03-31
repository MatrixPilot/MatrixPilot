
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

logoInstructionDef_t* logo_compile(uint16_t* count, const char* source_filename);
uint16_t logo_save(logoInstructionDef_t* logo, uint16_t count, const char* logo_filename);
uint16_t logo_save_hex(logoInstructionDef_t* logo, uint16_t count, const char* logo_filename);
