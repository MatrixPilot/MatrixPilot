// This file is part of MatrixPilot.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009-2011 MatrixPilot Team
// See the AUTHORS.TXT file for a list of authors of MatrixPilot.
//
// MatrixPilot is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// MatrixPilot is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with MatrixPilot.  If not, see <http://www.gnu.org/licenses/>.


#include "../MatrixPilot/defines.h"
#include "minifloat.h"

// Long to minifloat
minifloat ltomf(long n)
{
    minifloat mf = {0,0};

    if(n==0)
        return mf;

    // Special case for -1 which is 0xFFFFFFFFs
    // It has no zeros so we can't search for them
    if(n==-1)
    {
        mf.mant = MANT_NEG_MIN;
        mf.exp = 1;
        return mf;
    }

    union longww temp = {0};
    temp.WW = n;
    int index = 0;

    // Find the index of the first bit
    // Search bit 1 for positive and bit 0 for negative
    if(n>0)
    {
        while( (temp._.W1 & 0x4000) == 0)
        {
            index++;
            temp.WW <<= 1;
        }
    }
    else
    {
        while( (temp._.W1 & 0x4000) == 1)
        {
            index++;
            temp.WW <<= 1;
        }
    }

    temp._.W1 >>= 7;
    mf.mant = temp._.W1;
    mf.exp = 32 - index;

    return mf;
};


// Multiply two minifloats
minifloat mf_mult(minifloat a, minifloat b)
{
    minifloat mf = {0,0};
    union longww temp = {0};

    // Check for zero mantissas
    if(a.mant == 0) return mf;
    if(b.mant == 0) return mf;

    // Scale manitssas to RMAX scale
	temp._.W0 = ((int) a.mant) << 6;
	temp._.W1 = ((int) b.mant) << 6;
    temp.WW =  __builtin_mulss ( temp._.W0 , temp._.W1) << 2;    
    if(temp._.W0 & 0x8000) temp._.W1++;

    // Check if the result is under 0.25 = RMAX/2
    // Correct mant and exp if necessary;
    if(temp.WW > 0)
    {
        if(temp.WW < RMAX/2)
        {
            temp.WW <<= 1;
            mf.exp = -1;
        }
    }
    else
    {
        if(temp.WW > -RMAX/2)
        {
            temp.WW <<= 1;
            mf.exp = -1;
        }
    }
    
    mf.exp += (a.exp + b.exp);
    mf.mant = temp._.W1 >> 6;

    return mf;
}

minifloat mf_sqrt(minifloat num)
{
    union longww temp = {0};
    minifloat mf = {0,0};
    
    // return zero on zero or negative number
    if(num.mant <= 0) return mf;

    // Divide exponent
    mf.exp = num.exp >> 1;

    // Multiplied by 16 before sqrt
    temp._.W1 = num.mant;
    temp.WW = sqrt_long(temp.WW);
    // Now divide by 8.  Multiply and take high word to keep accuracy
    temp.WW <<= 8;

    // If the exponent is odd, correct by 1/SQRT(2)
    if(num.exp & 0x1)
    {
        temp.WW =  __builtin_mulss ( temp._.W1 , RMAX / 1.4142135624 ) << 2;
        mf.exp++;
    }

    // TODO Need correction for sqrt overflow
    mf.mant = temp._.W1;
	return mf;
}


// Square of a minifloat
minifloat mf_sqr(minifloat num)
{
    minifloat mf = {0,0};
    union longww temp = {0};

    // Check for zero mantissas
    if(num.mant == 0) return mf;

    // Scale manitssas to RMAX scale
	temp._.W0 = ((int) num.mant) << 6;
    temp.WW =  __builtin_mulss ( temp._.W0 , temp._.W0) << 2;    
    if(temp._.W0 & 0x8000) temp._.W1++;

    // Check if the result is under 0.25 = RMAX/2
    // Correct mant and exp if necessary;
    if(temp.WW > 0)
    {
        if(temp.WW < RMAX/2)
        {
            temp.WW <<= 1;
            mf.exp = -1;
        }
    }
    else
    {
        if(temp.WW > -RMAX/2)
        {
            temp.WW <<= 1;
            mf.exp = -1;
        }
    }
    
    mf.exp += (num.exp >> 1);
    mf.mant = temp._.W1 >> 6;

    return mf;
}


minifloat mf_div(minifloat num, minifloat den)
{
    minifloat mf = {0,0};
    union longww temp = {0};
	
	// Scale numerator and denominator to RMAX
	temp._.W1 = ((int) num.mant) << 4; // (6-2)
	fractional denom = ((int) den.mant) << 6;

	temp._.W1 = __builtin_divsd(temp.WW, denom);
	mf.mant = temp._.W1 >> 6;
	mf.exp = num.exp - den.exp;

	return mf;
}


