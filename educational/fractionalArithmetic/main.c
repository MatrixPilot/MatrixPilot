/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

/* Device header file */
#if defined(__dsPIC33E__)
#include <p33Exxxx.h>
#elif defined(__dsPIC33F__)
#include <p33Fxxxx.h>
#endif

#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */
#include <libq.h>
#include "libDCM.h"

#include "system.h"        /* System funct/params, like osc/peripheral config */

_FOSCSEL(FNOSC_PRIPLL); // pri plus PLL (primary osc  w/ PLL)
_FOSC(FCKSM_CSDCMD &
      OSCIOFNC_OFF &
      POSCMD_XT); // Clock switching is enabled, Fail-Safe Clock Monitor is disabled,
// OSC2 pin has clock out function
// Primary Oscillator XT mode

_FWDT(FWDTEN_OFF &
      WINDIS_OFF); // Watchdog timer enabled/disabled by user software
// Watchdog Timer in Non-Window mode
_FGS(GSS_OFF &
     GCP_OFF &
     GWRP_OFF); // User program memory is not code-protected
// User program memory is not write-protected
_FPOR(FPWRT_PWR1); // POR Timer Value: Disabled
_FICD(JTAGEN_OFF &
      ICS_PGD2); // JTAG is Disabled

/******************************************************************************/
/* Global Variable Declaration                                                */
/******************************************************************************/

// assembly language function addSat
extern int addSat(int, int);

/******************************************************************************/
/* Main Program                                                               */

/******************************************************************************/

int16_t main(void) {
    //  from rmat.c:
    //	All rmat coefficients are stored in Q2.14 format: range [-2,2)
    //	Vector and matrix libraries work in Q1.15 format: range [-1,1)
    //	Multiplication using builtin routine mul.ss produces results scaled by 1/2.
    //  Note: unsigned 0.16 format range is [0,1)

    // unsigned Q0.16 format: 0. 2^-1, 2^-2, ... 2^-16
    // signed Q1.15 format: Sign bit =2^0. 2^-1, ..., 2^-15
    // product of 2 signed Q1.15 fractionals has 2 bits to the left of the radix point: format Q2.30
    // product of 2 signed Q2.14 fractionals has 4 bits to the left of the radix point: format Q4.28

    // This is the reason for the shift in radix point location when multiplying
    // two Q2.14 numbers. (this does not happen with unsigned Q0.16 numbers)
    // The 32 bit product of two Q2.14 numbers: Q2.14 * Q2.14 = Q4.28
    // has 4 bits to left of decimal point and 28 fractional bits. range is [-8,8)

    // The 32 bit product of two unsigned 16 bit Q0.16 fractional integers is
    // Q0.32 with the radix point in the same location as before, left of the MSB.

    // Also, the 32 bit product of two unsigned 16 bit Q16.0 integers is
    // Q32.0 with the radix point in the same location as before, right of the LSB.

    // RMAX = 2^14 = 0x4000 = 0b0100 0000 0000 0000
    // RMAX * RMAX = 2^28 = 0x1000 0000
    long rmax_rmax = __builtin_mulss(RMAX, RMAX); // should be 1 in 4.30 format
    // actual result is 0x1000 0x0000 = 2^28 = 1 in 4.30 format
    // to get back to 2.14 format, we must left shift the high word by two bits

    int umax = 0xFFFF; // 1 in 0.16 format
    int smax = 0x7FFF; // 1 in 1.15 format
    int smin = 0x8000; // -1 in 1.15 format

    // smax = 1 - 2^-15 in Q1.15 format
    // smax * smax = 1 - 2^-14 + 2^-30 in Q2,30 format 
    long smax_smax = __builtin_mulss(smax, smax); // should be 1-2^-15 in 2.30 format
    // actual result is 0x3FFF 0x0001 = 1 - 2^-14 + 2^-30 in 2.30 format
    // Must shift left by 1 bit to get back to Q1.15 format

    long smin_smax = __builtin_mulss(smin, smax); // should be -(1-2^-15) in 2.14 format
    // actual result is 0xC000 0x8000 = -(1 - 2^-15) in 2.30 format

    long smin_smin = __builtin_mulss(smin, smin); // should be 1-2^-15 in 2.30 format
    // actual result is 0x4000 0x0000 = 1 in 2.30 format

    int quotient = __builtin_divsd(smax_smax, smax);
    quotient = __builtin_divsd(smax_smax, smin);

    // product of 0.16 (unsigned) and 1.15 has 1 sign bit => result is 1.15

    long umax_smax = __builtin_mulus(umax, smax); // should be 1-2^-15 in 1.15 format
    // actual result is 0x7FFE, 0x8001 = 1-2^-15 + 2^-16 + 2^-31 in 1.15 format

    long umax_smin = __builtin_mulus(umax, smin); // should be -1 in 1.15 format
    // 0x8000 0x8000 = -(1 - 2^-16) in 1.15 format
    // magnitude of result is 0x7FFF 8000 = 1 - 2^-16

    // product of 2 unsigned fractionals => result is 0.16

    long umax_umax = __builtin_muluu(umax, umax); // 1-2^-16 in 0.16 format
    //0xFFFE 0x0001 = 1 - 2^-16 + 2^-32 in 0.16 format

    // compute 2 * 1
    int two_smax = __builtin_mulss(2, smax);
    // result is 0xFFFE (overflowed to negative)

    // test saturating logic on 16 bit int
    // CORCON bits SATA, SATB, SATDW control saturation
    // hover the mouse pointer over the name "CORCON" above to see its value in the simulator
    CORCON &= ~(1 << 7 | 1 << 6 | 1 << 5); // turn saturation off

    // compute smax + 2^-15 = +1
    int accAddTest = addSat(smax, 0x1);
    // result is smin (overflowed to negative)

    CORCON |= (1 << 7 | 1 << 6 | 1 << 5); // turn saturation off
    accAddTest = addSat(smax, 0x1);
    // result is smax (saturated)

    // the "q" library provides routines for performing saturating arithmetic
    // using fractional integers. These routines set CORCON themselves.
    // Microchip documenation is document DS51456G, "16-bit Language Tools Libraries".
    // Q16 refers to 16.16 32 bit format
    // Q15 refers to 1.15 format

    // compute -(-1)
    int testNegate = _Q15neg(smin); // result should be 0x7FFF (saturated)
    // note that _Q15neg works correctly whether SATDW is on or off

    // this overflows
    int badNegate = -smin;

    // -1 in Q15.16 format
    long lsmin = 0x80000000;

    // -(-1) = 1, no overflow
    long testOneL = _Q16neg(lsmin);

    // saturates at smin
    int testOvn = _Q15add(smin, -1);

    // saturates at smax
    int testOvp = _Q15add(smax, 1);

    // C30/XC16 representation of dsPIC 40 bit accumulators
    volatile register int accum_a asm("A");
    accum_a = __builtin_clr();
    accum_a = __builtin_mpy(4, 8, 0, 0, 0, 0, 0, 0);

    while (1) {
    }
}
