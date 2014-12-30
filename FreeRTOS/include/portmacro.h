//#ifdef _MSC_VER
#if (WIN == 1 || NIX == 1)

#include "../portable/MSVC-MingW/portmacro.h"

#else

#include "../portable/MPLAB/PIC24_dsPIC/portmacro.h"

#endif
