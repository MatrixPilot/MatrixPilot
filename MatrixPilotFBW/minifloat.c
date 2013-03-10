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

unsigned int sqrt_long_mf( unsigned long int sqr );
unsigned int sqrt_int_mf( unsigned int sqr );

// Long to minifloat
minifloat ltomf(long n)
{
    union longww temp = {0};
    minifloat mf = {0,0};
    int index = 0;

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

    if(n>0)
	    temp.WW = n;
	else
	    temp.WW = -n;

    // Find the index of the first bit
    // Search bit 1 for positive and bit 0 for negative

    while( (temp._.W1 & 0x4000) == 0)
	{
    	index++;
        temp.WW <<= 1;
   	}

	// Now the exponent has been found, shift back to correct
	// mantissa position for minifloat.
    temp._.W1 >>= 7;

	// TODO, underflow correction.

	if(n>0)
	    mf.mant = temp._.W1;
	else
		mf.mant = -temp._.W1;

    mf.exp = 31 - index;

    return mf;
};


//minifloat to long
long mftol(minifloat mf)
{
    union longww temp = {mf.mant};

	// Scale output to exponent.
	// Shift only works with positive parameter
	if(mf.exp >= 8)
		temp.WW <<= mf.exp - 8;
	else
		temp.WW >>= 8 - mf.exp;

	return temp.WW;
}


//minifloat to Q16 in longww union
// ._.W0 is underflow fractional
// ._.W1 is integer
extern _Q16 mftoQ16(minifloat mf)
{
    _Q16 temp = mf.mant;

	temp <<= mf.exp + 8;

	return temp;
}

// Q16 to minifloat
extern minifloat Q16tomf(_Q16 n)
{
    minifloat mf;
	mf = ltomf(n);
	mf.exp -= 16;

	return mf;
}

// Multiply two minifloats
minifloat mf_mult(minifloat a, minifloat b)
{
    minifloat mf = {0,0};
    union longww temp = {0};

    // Check for zero mantissas
    if(a.mant == 0) return mf;
    if(b.mant == 0) return mf;

    // Scale up manitssas to RMAX scale
	temp._.W0 = ((int) a.mant) << 6;
	temp._.W1 = ((int) b.mant) << 6;

	// Multiply and scale back down
    temp.WW =  __builtin_mulss ( temp._.W0 , temp._.W1) >> 4;

    // Check if the result is less than 0.5 fraction (or 128)
	// Correct if necessary
    if(temp.WW > 0)
    {
        if(temp._.W1 < 128)
        {
            temp.WW <<= 1;
            mf.exp = -1;
        }
    }
    else
    {
        if(temp._.W1 > -128)
        {
            temp.WW <<= 1;
            mf.exp = -1;
        }
    }

	// Adjust for underflow - TODO check if in right place.
    if(temp._.W0 & 0x8000) temp._.W1++;

	// Check for possible new overflow
    if(temp.WW > 0)
    {
        if(temp._.W1 >= 256)
        {
            temp.WW >>= 1;
            mf.exp += 1;
        }
    }
    else
    {
        if(temp._.W1 <= -256)
        {
            temp.WW >>= 1;
            mf.exp += 1;
        }
    }


    mf.exp += (a.exp + b.exp);
    mf.mant = temp._.W1;

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
    temp.WW = sqrt_long_mf(temp.WW);

    // If the exponent is odd, correct by 1/SQRT(2)
    if(num.exp & 0x1)
    {
        temp.WW =  __builtin_mulss ( temp._.W0 , RMAX / 1.4142135624 ) << 2;
		temp.WW = temp._.W1;
        mf.exp++;
    }
	else
		temp._.W1 = temp._.W0;

    // Now divide by 16.  Multiply and take high word to keep accuracy
    temp.WW <<= (16 - 4);
    if(temp._.W0 & 0x8000) temp._.W1++;

    mf.mant = temp._.W1;
	return mf;
}


//// Square of a minifloat
//minifloat mf_sqr(minifloat num)
//{
//    minifloat mf = {0,0};
//    union longww temp = {0};
//
//    // Check for zero mantissas
//    if(num.mant == 0) return mf;
//
//    // Scale manitssas to RMAX scale
//	temp._.W0 = ((int) num.mant) << 6;
//    temp.WW =  __builtin_mulss ( temp._.W0 , temp._.W0) << 2;    
//    if(temp._.W0 & 0x8000) temp._.W1++;
//
//    // Check if the result is under 0.25 = RMAX/2
//    // Correct mant and exp if necessary;
//    if(temp.WW > 0)
//    {
//        if(temp.WW < RMAX/2)
//        {
//            temp.WW <<= 1;
//            mf.exp = -1;
//        }
//    }
//    else
//    {
//        if(temp.WW > -RMAX/2)
//        {
//            temp.WW <<= 1;
//            mf.exp = -1;
//        }
//    }
//    
//    mf.exp += (num.exp << 1);
//    mf.mant = temp._.W1 >> 6;
//
//    return mf;
//}
//

minifloat mf_div(minifloat num, minifloat den)
{
    minifloat mf = {0,0};
    union longww temp = {0};

	// Check for zero numerator
	if(num.mant == 0) return mf;

	// Check for zero denominator
	if(den.mant == 0) return mf;
	
	// Scale numerator and denominator to RMAX
	temp._.W1 = ((int) num.mant) << 4; // (6-3?)
	fractional denom = ((int) den.mant) << 6;

	temp._.W1 = __builtin_divsd(temp.WW, denom);
	temp.WW >>= 6;

	if(temp._.W1 >= 0)
	{
		if(temp._.W1 >= 0x100)
		{
			temp.WW >>= 1;
			mf.exp = 1;
		}
	}
	else
	{
		if(temp._.W1 <= -0x100)
		{
			temp.WW >>= 1;
			mf.exp = 1;
		}
	}

    if(temp._.W0 & 0x8000) temp._.W1++;

	mf.mant = temp._.W1;
	mf.exp += (num.exp - den.exp);

	return mf;
}


// Add minifloats
minifloat mf_add(minifloat a, minifloat b)
{
	minifloat mf = {0,0};
	int expdiff;
	int mant;
	minifloat larger;
	minifloat smaller;

	if(a.mant == 0) return b;
	if(b.mant == 0) return a;

	// Select which one to range
	if(a.exp >= b.exp)
	{
		larger = a;
		smaller = b;
	}
	else
	{
		larger = b;
		smaller = a;
	}

	expdiff = larger.exp - smaller.exp;
	if(expdiff > 9) return larger;

	mant = larger.mant;
	mant += ((int) smaller.mant) >> expdiff;

	// test special case where result is zero
	if(mant == 0) 
		return mf;

	mf.exp = larger.exp;

	if(mant > 0)
	{
		while(mant >= 256)
		{
			mant >>= 1;
			mf.exp += 1;
		}

		while(mant < 128)
		{
			mant <<= 1;
			mf.exp -= 1;
		}
	}
	else
	{
		while(mant <= -256)
		{
			mant >>= 1;
			mf.exp += 1;
		}
		
		while(mant > -128)
		{
			mant <<= 1;
			mf.exp -= 1;
		}
	}

	mf.mant = mant;

	return mf;
}

// Subtract minifloat a-b
extern minifloat mf_sub(minifloat a, minifloat b)
{
	minifloat mf = b;
	mf.mant = -mf.mant;
	return mf_add(a, mf);
}


// Floating point to minifloat
minifloat ftomf(float num)
{
	minifloat mf = {0,0};
	int expon;
	float fmant;
	
	fmant = frexp(num, &expon);
	fmant *= 256;
	mf.mant = (int) fmant;
	mf.exp = expon;

	return mf;
}

float mftof(minifloat num)
{
	float mant = ((int) num.mant);
	int expon = (int) num.exp;

	mant /= 256.0;

	mant = ldexp(mant, expon);
	return mant;
}

// RMAX scale to minifloat
minifloat RMAXtomf(fractional num)
{
	minifloat mf;
	mf = ltomf(num);
	mf.exp -= 14;
	return mf;
}


// HELPER FUNCTIONS

unsigned int sqrt_long_mf( unsigned long int sqr )
{
	// based on Heron's algorithm
	unsigned int binary_point = 0 ;
	unsigned int result = 65535 ; // need to start high and work down to avoid overflow in divud

	int iterations = 3 ;	// thats all you need

	if ( sqr < 65536 )	// use the 16 bit square root
	{
		return sqrt_int_mf( ( unsigned int ) sqr ) ;
	}
	while ( ( sqr & 0xC0000000 ) == 0 ) // shift left to get a 1 in the 2 MSbits
	{
		sqr = sqr<< 2 ;
		binary_point ++ ; // track half of the number of bits shifted
	}
	sqr = sqr>>1 ; // for convenience, Herons formula is result = ( result + sqr/result ) / 2
	while ( iterations )
	{
		iterations -- ;
		result = result/2 + __builtin_divud ( sqr , result ) ;
	}
	result = result >> binary_point ; // shift result right to account for shift left of sqr 
	return result ;
}


unsigned int sqrt_int_mf( unsigned int sqr )
{
	// based on Heron's algorithm
	unsigned int binary_point = 0 ;
	unsigned int result = 255 ; 
							
	int iterations = 3 ;		
	if ( sqr == 0 )
	{
		return 0 ;
	}
	while ( ( sqr & 0xC000 ) == 0 ) // shift left to get a 1 in the 2 MSbits
	{
		sqr = sqr*4 ; // shift 2 bits
		binary_point ++ ; // track half of the number of bits shifted
	}
	sqr = sqr/2 ; // for convenience, Herons formula is result = ( result + sqr/result ) / 2
	while ( iterations )
	{
		iterations -- ;
		result = result/2 + sqr/result ;
	}
	result = result >> binary_point ; // shift result right to account for shift left of sqr 
	return result ;
}

