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

uint16_t sqrt_long_mf( uint32_t int16_t sqr );
uint16_t sqrt_int_mf( uint16_t sqr );

// Long to minifloat
minifloat ltomf(int32_t n)
{
    union longww temp = {0};
    minifloat mf = {0,0};
    int16_t index = 0;

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
int32_t mftol(minifloat mf)
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

	int16_t expon = (a.exp + b.exp);

    // Check for zero mantissas
    if(a.mant == 0) return mf;
    if(b.mant == 0) return mf;

	// Check if result is too small and return zero if so
	if(expon < -62) 
		return mf;

	// Check if result is too large and return full scale if so
	if(expon > 62)
	{
		mf.mant = 0xFF;
		mf.exp = 63;
		return mf;
	}

    // Scale up manitssas to RMAX scale
	temp._.W0 = ((int16_t) a.mant) << 6;
	temp._.W1 = ((int16_t) b.mant) << 6;

	// Multiply and scale back down
    temp.WW =  __builtin_mulss ( temp._.W0 , temp._.W1) >> 4;

    // Check if the result is less than 0.5 fraction (or 128)
	// Correct if necessary
    if(temp.WW > 0)
    {
        if(temp._.W1 < 128)
        {
            temp.WW <<= 1;
            expon--;
        }
    }
    else
    {
        if(temp._.W1 > -128)
        {
            temp.WW <<= 1;
            expon--;
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
			expon++;
        }
    }
    else
    {
        if(temp._.W1 <= -256)
        {
            temp.WW >>= 1;
			expon++;
        }
    }


    mf.exp = expon;
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


minifloat mf_div(minifloat num, minifloat den)
{
    minifloat mf = {0,0};
    union longww temp = {0};

	int16_t expon = (num.exp - den.exp);

	// Check for zero numerator
	if(num.mant == 0) return mf;

	// Check for zero denominator
	if(den.mant == 0) return mf;

	// Check if result is too small or too large
	if(expon < -62) 
		return mf;
	else if(expon > 62) 
		return mf;
	
	// Scale numerator and denominator to RMAX
	temp._.W1 = ((int16_t) num.mant) << 4; // (6-3?)
	fractional denom = ((int16_t) den.mant) << 6;

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
	int16_t expdiff;
	int16_t mant;
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
	mant += ((int16_t) smaller.mant) >> expdiff;

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

// Compare magnitude of b to magnitude of a
// If greater, return 1.  If less return -1
// If equal return 0
int16_t mf_compare_mag(minifloat a, minifloat b)
{
	int16_t manta = a.mant;
	int16_t mantb = b.mant;

	if(b.exp > a.exp) return 1;
	if(b.exp < a.exp) return -1;

	if(b.mant == a.mant) return 0;

	if(manta < 0) manta = -manta;
	if(mantb < 0) mantb = -mantb;

	if(mantb > manta) return 1;
	return -1;
}

// Get magnitude of minifloat
minifloat mf_mag(minifloat num)
{
	minifloat mf;
	mf.exp = num.exp;

	if(num.mant < 0) 
		mf.mant = -num.mant;
	else
		mf.mant = num.mant;
	return mf;
}

// Check if b is larger than a
int16_t mf_larger(minifloat a, minifloat b)
{
	if( (b.mant >= 0) && (a.mant < 0) )
		return 1;

	if( (b.mant < 0) && (a.mant >= 0) )
		return -1;

	if( (b.mant > 0) && (a.mant > 0) )
	{
		if(b.exp > a.exp)
			return 1;
		if(b.exp < a.exp)
			return -1;
		if(b.mant > a.mant)
			return 1;
		if(b.mant < a.mant)
			return -1;
		return 0;
	}
	else if( (b.mant < 0) && (a.mant < 0) )
	{
		if(b.exp < a.exp)
			return 1;
		if(b.exp > a.exp)
			return -1;
		if(b.mant < a.mant)
			return 1;
		if(b.mant > a.mant)
			return -1;
		return 0;
	}

	return 0;
}

// Invert
inline minifloat mf_inv(minifloat num)
{
	minifloat mf = num;
	mf.mant = -num.mant;
	return mf;
}


// Floating point to minifloat
minifloat ftomf(float num)
{
	minifloat mf = {0,0};
	int16_t expon;
	float fmant;
	
	fmant = frexp(num, &expon);
	fmant *= 256;
	mf.mant = (int16_t) fmant;
	mf.exp = expon;

	return mf;
}

float mftof(minifloat num)
{
	float mant = ((int16_t) num.mant);
	int16_t expon = (int16_t) num.exp;

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

uint16_t sqrt_long_mf( uint32_t int16_t sqr )
{
	// based on Heron's algorithm
	uint16_t binary_point = 0 ;
	uint16_t result = 65535 ; // need to start high and work down to avoid overflow in divud

	int16_t iterations = 3 ;	// thats all you need

	if ( sqr < 65536 )	// use the 16 bit square root
	{
		return sqrt_int_mf( ( uint16_t ) sqr ) ;
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


uint16_t sqrt_int_mf( uint16_t sqr )
{
	// based on Heron's algorithm
	uint16_t binary_point = 0 ;
	uint16_t result = 255 ; 
							
	int16_t iterations = 3 ;		
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

