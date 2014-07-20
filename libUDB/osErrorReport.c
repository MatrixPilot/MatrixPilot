#include "uart.h"

extern  unsigned int    wRegSave[16];
extern  unsigned int    TrapAddr;
extern  unsigned int    ErrorAddrHigh;
extern  unsigned int    ErrorAddrLow;
extern  unsigned int    _SP_init;
extern  unsigned int    _dsr_pag;
extern  unsigned int    _dsw_pag;
extern  unsigned int    _tbl_pag;

int SP_limit(void);
 
#define SPDUMP_MAX 256          // Maximum stack to dump

static const char * const trapMsg = {                               
	"Reset" "\x0"  //0x0 -- offset 0
	"Osc. " "\x0"  //0x2 -- offset 6
	"Addr." "\x0"  //0x4 -- offset 12
	"Hard " "\x0"  //0x6 -- offset 18
	"Stack" "\x0"  //0x8 -- offset 24
	"Math " "\x0"  //0xA -- offset 30
	"DMAC " "\x0"  //0xC -- offset 36
	"Soft " "\x0"  //0xE -- offset 42
};

 
void osError_Report(void)
{
	int i;
	unsigned int * ptr;

	Puts("\r\nTrap to 0x");
	PutHexWord((unsigned char)TrapAddr);
	if (TrapAddr < 0x10) {      // say the common name
		Puts(" {");
		if (TrapAddr >= 0x0) {
			Puts(trapMsg + (TrapAddr * 3));
		}
		PutChar('}');
	}
	Puts(" from 0x");
	PutHexNibble(ErrorAddrHigh & 0x7);    // say 0x9 (a nibble)
	PutHexWord(ErrorAddrLow & 0xFFFE);    // mask away low bit if set
	for (i = 0; i < 16; i++) {
		if ((i & 7) == 0) {
			Puts("\r\n   w");
			PutChar((char)('0' + i));
			PutChar(':');
		}
		Puts(" 0x");
		PutHexWord(wRegSave[i]);
	}
	Puts("\r\ndsrpag 0x");
	PutHexWord(_dsr_pag);
	Puts(" dswpag 0x");
	PutHexWord(_dsw_pag);
	Puts(" tblpag 0x");
	PutHexWord(_tbl_pag);

	Puts("\r\nStack: 0x");
	PutHexWord((unsigned int)&_SP_init);
	Puts(" to 0x");
	PutHexWord(wRegSave[15]);
	Puts(" - limit: 0x");
	PutHexWord(SP_limit());

	ptr = &_SP_init;
	for (i = 0; i < SPDUMP_MAX; i++) {
		if ((((int)ptr & 15) == 0) || (i == 0)) {
			Puts("\r\n ");
			PutHexWord((unsigned int)ptr);
			PutChar(':');
		}
		Puts(" 0x");
		PutHexWord(*ptr++);
//		if ((unsigned int *)ptr >= (unsigned int *)wRegSave[15]) break;
	}
//	Puts(" <-caller");       // Remind us where to start looking

//	Puts("\r\ncaller ");       // Remind us where to start looking
//	PutHexWord(*(unsigned int*)wRegSave[15]);

//	asm("reset");
	for (;;);
}
