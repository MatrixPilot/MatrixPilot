/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

/* Device header file */
#if defined(__XC16__)
    #include <xc.h>
#elif defined(__C30__)
    #if defined(__dsPIC33E__)
    	#include <p33Exxxx.h>
    #elif defined(__dsPIC33F__)
    	#include <p33Fxxxx.h>
    #endif
#endif

#include <stdint.h>        /* Includes uint16_t definition */
#include <stdbool.h>       /* Includes true/false definition */

/******************************************************************************/
/* Trap Function Prototypes                                                   */
/******************************************************************************/

/* <Other function prototypes for debugging trap code may be inserted here>   */

/* Use if INTCON2 ALTIVT=1 */
void __attribute__((interrupt,no_auto_psv)) _OscillatorFail(void);
void __attribute__((interrupt,no_auto_psv)) _AddressError(void);
void __attribute__((interrupt,no_auto_psv)) _StackError(void);
void __attribute__((interrupt,no_auto_psv)) _MathError(void);

#if defined(__HAS_DMA__)

void __attribute__((interrupt,no_auto_psv)) _DMACError(void);

#endif

#if defined(__dsPIC33F__)

/* Use if INTCON2 ALTIVT=0 */
void __attribute__((interrupt,no_auto_psv)) _AltOscillatorFail(void);
void __attribute__((interrupt,no_auto_psv)) _AltAddressError(void);
void __attribute__((interrupt,no_auto_psv)) _AltStackError(void);
void __attribute__((interrupt,no_auto_psv)) _AltMathError(void);

    #if defined(__HAS_DMA__)

    void __attribute__((interrupt,no_auto_psv)) _AltDMACError(void);

    #endif

#endif

/* Default interrupt handler */
void __attribute__((interrupt,no_auto_psv)) _DefaultInterrupt(void);

#if defined(__dsPIC33E__)

/* These are additional traps in the 33E family.  Refer to the PIC33E
migration guide.  There are no Alternate Vectors in the 33E family. */
void __attribute__((interrupt,no_auto_psv)) _HardTrapError(void);
void __attribute__((interrupt,no_auto_psv)) _SoftTrapError(void);

#endif

/******************************************************************************/
/* Trap Handling                                                              */
/*                                                                            */
/* These trap routines simply ensure that the device continuously loops       */
/* within each routine.  Users who actually experience one of these traps     */
/* can add code to handle the error.  Some basic examples for trap code,      */
/* including assembly routines that process trap sources, are available at    */
/* www.microchip.com/codeexamples                                             */
/******************************************************************************/

/* Primary (non-alternate) address error trap function declarations */
void __attribute__((interrupt,no_auto_psv)) _OscillatorFail(void)
{
        INTCON1bits.OSCFAIL = 0;        /* Clear the trap flag */
        while(1);
}

void __attribute__((interrupt,no_auto_psv)) _AddressError(void)
{
        INTCON1bits.ADDRERR = 0;        /* Clear the trap flag */
        while (1);
}
void __attribute__((interrupt,no_auto_psv)) _StackError(void)
{
        INTCON1bits.STKERR = 0;         /* Clear the trap flag */
        while (1);
}

void __attribute__((interrupt,no_auto_psv)) _MathError(void)
{
        INTCON1bits.MATHERR = 0;        /* Clear the trap flag */
        while (1);
}

#if defined(__HAS_DMA__)

void __attribute__((interrupt,no_auto_psv)) _DMACError(void)
{
        INTCON1bits.DMACERR = 0;        /* Clear the trap flag */
        while (1);
}

#endif

#if defined(__dsPIC33F__)

/* Alternate address error trap function declarations */
void __attribute__((interrupt,no_auto_psv)) _AltOscillatorFail(void)
{
        INTCON1bits.OSCFAIL = 0;        /* Clear the trap flag */
        while (1);
}

void __attribute__((interrupt,no_auto_psv)) _AltAddressError(void)
{
        INTCON1bits.ADDRERR = 0;        /* Clear the trap flag */
        while (1);
}

void __attribute__((interrupt,no_auto_psv)) _AltStackError(void)
{
        INTCON1bits.STKERR = 0;         /* Clear the trap flag */
        while (1);
}

void __attribute__((interrupt,no_auto_psv)) _AltMathError(void)
{
        INTCON1bits.MATHERR = 0;        /* Clear the trap flag */
        while (1);
}

    #if defined(__HAS_DMA__)

    void __attribute__((interrupt,no_auto_psv)) _AltDMACError(void)
    {
         INTCON1bits.DMACERR = 0;        /* Clear the trap flag */
         while (1);
    }

    #endif

#endif

/******************************************************************************/
/* Default Interrupt Handler                                                  */
/*                                                                            */
/* This executes when an interrupt occurs for an interrupt source with an     */
/* improperly defined or undefined interrupt handling routine.                */
/******************************************************************************/
void __attribute__((interrupt,no_auto_psv)) _DefaultInterrupt(void)
{
        while(1);
}

#if defined(__dsPIC33E__)

/* These traps are new to the dsPIC33E family.  Refer to the device Interrupt
chapter of the FRM to understand trap priority. */
void __attribute__((interrupt,no_auto_psv)) _HardTrapError(void)
{
    while(1);
}
void __attribute__((interrupt,no_auto_psv)) _SoftTrapError(void)
{
    while(1);
}

#endif
