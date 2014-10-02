//#include <setjmp.h>
//#include "unity.h"
//#include "udbTypes.h"
//#include "dcmTypes.h"


#if (WIN == 1 || NIX == 1)
#include "SIL-udb.h"
int mp_argc;
char **mp_argv;
#endif // (WIN == 1 || NIX == 1)


void sil_ui_init(uint16_t mp_rcon) {}
void sil_ui_update(void) {}
void sil_ui_will_reset(void) {}
