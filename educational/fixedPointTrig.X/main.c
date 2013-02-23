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
#include <time.h>
#include "libDCM.h"

#include "system.h"        /* System funct/params, like osc/peripheral config */

/******************************************************************************/
/* Main Program                                                               */

/******************************************************************************/
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

int16_t main(void)
{
    clock_t now, then;
    // _Q15cosPI: domain is [-pi, pi), range [-1,1)
    int v;
    then = clock();
    v = _Q15cosPI(0);
    now = clock();

    v = _Q15cosPI(0.125 * 32768);
    then = clock(); // 84 cycles
    v = _Q15cosPI(0.25 * 32768);
    now = clock();  // 84 cycles

    // _Q15sinPI: domain is [-pi, pi), range [-1,1)
    v = _Q15sinPI(0);
    v = _Q15sinPI(0.125 * 32768);
    v = _Q15sinPI(0.25 * 32768);

    // _Q15asin is a half-quadrant arcsine with valid domain [sin(-pi/4), sin(pi/4)]
    int asinZero = _Q15asin(0); // asin(0)
    int asin_9 = _Q15asin(32768 * .15643); // asin(sin(pi/20)): 66 cycles
    int asin_M22_5 = _Q15asin(32768 * -.38268); // asin(sin(-pi/8)): 66 cycles
    int asin_M45 = _Q15asin(32768 * -.70711); // asin(sin(-pi/4)): 26 cycles
    int asin_22_5 = _Q15asin(32768 * .38268); // asin(sin(pi/8))
    int asin_45 = _Q15asin(32768 * .70711); // asin(sin(pi/4))
    // *** out of range inputs
    int asin_48 = _Q15asin(32768 * .84145); // asin(sin(.31830 pi)) returns .31828 pi
    int asin_60 = _Q15asin(32768 * .86603); // asin(sin(pi/3)) !!! not correct; returns .99997 instead of 1.0472

    // _Q16asin is a half-quadrant arcsine with valid domain [sin(-pi/4), sin(pi/4)]
    long asinZeroL = _Q16asin(0);
    long asin_9L = _Q16asin(65536 * .15643); // asin(sin(pi/20)): 66 cycles, result .15707
    long asin_M22_5L = _Q16asin(65536 * -.38268); // asin(sin(-pi/8)): 66 cycles, result
    long asin_M45L = _Q16asin(65536 * -.70711); // asin(sin(-pi/4)): 26 cycles
    long asin_22_5L = _Q16asin(65536 * .38268); // asin(sin(pi/8)), result pi/8
    long asin_45L = _Q16asin(65536 * .70711); // asin(sin(pi/4)), result pi/4
    // *** out of range inputs
    long asin_48L = _Q16asin(65536 * .84145); // asin(sin(.31830 pi)) returns .31830 pi
    long asin_60L = _Q16asin(65536 * .86603); // asin(sin(pi/3)), incorrect: result 69.3 deg
    long asin_70L = _Q16asin(65536 * .93969); // asin(sin(70 deg)), incorrect: result 69.996 deg
    long asin_80L = _Q16asin(65536 * .98481); // asin(sin(80 deg)), incorrect: result 79.997 deg
    long asin_90L = _Q16asin(65536); // asin(sin(pi/2)), incorrect: result pi/2

    long asin16[360];
    int ii;
    for (ii = 0; ii < 360; ii++)
    {
        // run sine linearly from -1 to 1 in Q16.15 format
        long stheta = -65536 + 65536 * ii / 180;
        asin16[ii] = _Q16asin(stheta);
    }


    struct relative2D matrix_accum;
    matrix_accum.y = 0.707 * RMAX;
    matrix_accum.x = 0.707 * RMAX;
    // 1033 cycles in simulator
    int angle_r2p = rect_to_polar16(&matrix_accum); // binary angle (0 - 65536 = 360 degrees)

    matrix_accum.y = 0.5 * RMAX;
    matrix_accum.x = 0.866 * RMAX;
    // 1030 cycles in simulator
    angle_r2p = rect_to_polar16(&matrix_accum); // binary angle (0 - 65536 = 360 degrees)

    matrix_accum.y = 0.92388 * RMAX;
    matrix_accum.x = 0.38268 * RMAX;
    // 1033 cycles in simulator
    angle_r2p = rect_to_polar16(&matrix_accum); // binary angle (0 - 65536 = 360 degrees)

    // 23 cycles in sim: pi/4
    long angle_q16 = _Q16atan(65536);
    // result is .25 pi

    // 370 cycles in simulator: pi/6
    angle_q16 = _Q16atan((0.5 / 0.866) * 65536);
    // result is .16666 pi

    // pi/6.1
    angle_q16 = _Q16atan(0.56596 * 65536);
    // result is pi/6.1005

    // pi/2: 38 cycles
    angle_q16 = _Q16atan(0x7FFFFFFF);
    // result is .49999 pi

    // -pi/2: 15 cycles
    angle_q16 = _Q16atan(0x80000000);
    // result is -.49999 pi

    // .23 * pi
    long tangent = .88162 * 65536;
    angle_q16 = _Q16atan(tangent);
    // result is .24425 * pi

    // .24 * pi
    tangent = .93906 * 65536;
    angle_q16 = _Q16atan(tangent);
    // result is .26017 * pi

    // .26 * pi
    tangent = 1.0649 * 65536;
    long angle_26pi = _Q16atan(tangent);
    // result is 0x11B72 => .35244 pi

    // 228 cycles in simulator: pi/3
    tangent = 1.7321 * 65536;
    angle_q16 = _Q16atan(tangent);
    // result is 0x11B72 => .35244 pi = pi/2.8374

    // 254 cycles in simulator: 3pi/8 = .375 pi
    tangent = 2.4142 * 65536;
    angle_q16 = _Q16atan(tangent);
    // result is 0x1536C => .42204 pi

    int omega2 = 45 * DEGPERSEC;

    union longww omega_z; // result in rad/sec, decimal point between words
    omega_z.WW = __builtin_mulus((unsigned int) (65536 * (PI / 180) / DEGPERSEC), omega2);

    long rotLong;

    // _Q15atan is a 2-octant fixed point arctangent
    // range is [-pi/4, pi/4) radians
    // domain is [-1, 1)

    // _Q16atan is a 2 quadrant 32 bit arctan
    // domain is [-2^15, 2^15-1]
    // result is Q16 radians, range (-pi/2, pi/2)
    //    omega_z.WW = 0x80000000;
    rotLong = _Q16atan(omega_z.WW);

    // convert to 8 bit byte circular assuming a range of +/-pi/2 radians
    // rotWord is radians * 2^14
    int rotWord = 0xFFFF & (rotLong >> 2);
    union longww rotAngle;
    signed char precessBC;
    // lagAngle high word is radians * 2^14/PI
    rotAngle.WW = __builtin_mulus((unsigned int) (65536 / PI), rotWord);
    // lagBC is in byte circular form; should wrap correctly at 360 degrees
    precessBC = 0xFF & (rotAngle._.W1 >> 6);

    // test fixed point arctangent function
    // domain is [-1, 1), range is [-pi/4, pi/4) radians
    int thetamin = _Q15atan(0x8000);
    int thetamax = _Q15atan(0x7FFF);
    int theta1 = _Q15atan(.88162 * 32768); // .23 pi, result .22999 pi, 244 cycles
    theta1 = _Q15atan(.72654 * 32768); // .2 pi, result .19999 pi, 150 cycles
    theta1 = _Q15atan(.93696 * 32768); // .24 pi, result .23964 pi
    theta1 = _Q15atan(.32492 * 32768); // .1 pi, result .099987 pi

    while (1)
    {
    }
}
