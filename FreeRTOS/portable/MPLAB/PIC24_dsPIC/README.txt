This directory contains the two files for the 16bit PIC microprocessors from Microchip
it covers the PIC24F, PIC24FK, PIC24H, PIC24E, dsPIC30F, dsPIC33E and dsPIC33F
(these are all the families I know of).

Compared to the previous version this does require any more than placing in your project.
It is no longer dependent on MPLAB project settings for the dsPICs and works fine with standalone pic30-gcc
of xc16-gcc.

The configKERNEL_INTERRUPT_PRIORITY macro can now be changed in FreeRTOSConfig.h without changing
port.c

In addition this version adds hardware stack checking through the use of SPLIM. The user is responsible
for adding a hook to deal with a stack overflow. ie define

void __attribute__ ((__interrupt__, __auto_psv__)) _StackError(void)
{
	... help! ...
}

The stack trap happens 16 words from the top - enough for the trap register push and a funtion call.
This can be changed in the top of portmacro.h by changing the macro traceTASK_CREATE.

I have tested this with the dsPIC33E, but it should be fine with all the other families
