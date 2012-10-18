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

#include "../../libUDB/filters.h"

/******************************************************************************/
/* Global Variable Declaration                                                */
/******************************************************************************/

#define ONE 65536
#define LPCB (ONE / 12)
//#define LPCA ((unsigned int)(ONE - LPCB))

// this function is similar to lp2 in libUDB/filters.c
inline int lp2a(int input, union longww *state)
{
    state->WW -= __builtin_mulus(LPCB, state->_.W1);
    // msb exponents are -1, 15, lsb exponents are -16, 0
    // result msb exponent is therefore 14, with lsb exponent -1
    state->WW += __builtin_mulus(LPCB, input);
    return (state->_.W1);
}

inline int lp2b(int input, int *output)
{
    *output = *output - (*output >> 4) + (input >> 4);
    return *output;
}

/******************************************************************************/
/* Main Program                                                               */

/******************************************************************************/

int16_t main(void)
{
    const int N = 3;
    const int len = 10;
    struct boxCarState filterState;
    int boxCarBuff[30];
    long boxCarSum[3];
    int boxCarAvg[3] = {0, 0, 0};
    int boxCarInput[3] = {10, 20, 30};

    init_boxCarState(len, N, boxCarBuff, boxCarSum, &filterState);

    int j, k;

    for (k = 0; k < N; k++)
    {
        for (j = 0; j < len; j++)
            boxCarBuff[len * k + j] = 0; //100 * k + j;
    }

    for (j = 0; j < 30; j++)
    {
        // run boxcar filter on new position
        boxcar(boxCarInput, &filterState, boxCarAvg);

        boxCarInput[0] += 0;
        boxCarInput[1] += 1;
        boxCarInput[2] += 0;
    }

    union longww lpState;
    int x[200], y[200], y2[200];
    int i, ym1 = 0;

    for (i = 0; i < 200; i++) x[i] = 0;
    for (i = 0; i < 200; i++) y[i] = 0;
    for (i = 0; i < 200; i++) y2[i] = 0;

    for (i = 0; i < 1; i++) x[i] = 32767;

    for (i = 0; i < 200; i++)
    {
        y[i] = lp2a(x[i], &lpState);
        y2[i] = lp2b(x[i], &ym1);
    }
    while (1)
    {
    }
}
