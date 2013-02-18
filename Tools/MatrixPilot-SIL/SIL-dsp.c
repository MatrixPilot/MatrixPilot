//
//  SIL-dsp.c
//  MatrixPilot-SIL
//
//  Created by Ben Levitt on 2/4/13.
//  Copyright (c) 2013 MatrixPilot. All rights reserved.
//

#include <stdio.h>
#include "SIL-dsp.h"


int16_t MatrixIndex(int16_t col, int16_t row, int16_t numCols)
{
	return col + row*numCols;
}


fractional fl2fr(float fl)
{
	return (int16_t)(fl*32768.0);
}


float fr2fl(fractional fr)
{
	return (fr/32768.0);
}


fractional* MatrixAdd (          /* Matrix addition */
					   /* dstM[i][j] =                 */
					   /*    srcM1[i][j] + srcM2[i][j] */
					   /* (in place capable) */
					   /* (with itself capable) */
					   int16_t numRows,                         /* number rows in srcM[1,2] (R) */
					   int16_t numCols,                         /* number columns in srcM[1,2] (C) */
					   fractional* dstM,                    /* ptr to destination matrix */
					   fractional* srcM1,                   /* ptr to source one matrix */
					   fractional* srcM2                    /* ptr to source two matrix */
					   
					   /* dstM returned */
					   )
{
	int16_t r, c;
	for (r=0; r< numRows; r++) {
		for (c=0; c< numCols; c++) {
			int16_t index = MatrixIndex(c, r, numCols);
			dstM[index] = fl2fr(fr2fl(srcM1[index]) + fr2fl(srcM2[index]));
		}
	}
	return dstM;
}


fractional* MatrixMultiply (     /* Matrix multiplication */
							/* dstM[i][j] =                 */
							/*    sum_k(srcM1[i][k]*srcM2[k][j]) */
							/* i in {0, 1, ..., numRows1-1} */
							/* j in {0, 1, ..., numCols2-1} */
							/* k in {0, 1, ..., numCols1Rows2-1} */
							/* (in place capable, only square) */
							/* (with itself capable, only square) */
							int16_t numRows1,                        /* number rows in srcM1 */
							int16_t numCols1Rows2,                   /* number columns in srcM1, same as */
							/* number rows in srcM2 */
							int16_t numCols2,                        /* number columns srcM2 */
							fractional* dstM,                    /* ptr to destination matrix */
							fractional* srcM1,                   /* ptr to source one matrix */
							fractional* srcM2                    /* ptr to source two matrix */
							
							/* dstM returned */
							)
{
	int16_t i, j, k;
	for (i=0; i < numRows1; i++) {
		for (j=0; j < numCols2; j++) {
			dstM[MatrixIndex(j, i, numCols2)] = 0;
			for (k=0; k < numCols1Rows2; k++) {
				dstM[MatrixIndex(j, i, numCols2)] += fl2fr(fr2fl(srcM1[MatrixIndex(k, i, numCols1Rows2)]) * fr2fl(srcM2[MatrixIndex(j, k, numCols2)]));
			}
		}
	}
	return dstM;
}


fractional* MatrixTranspose (
							 int numRows,
							 int numCols,
							 fractional* dstM,
							 fractional* srcM
							 )
{
	int16_t r, c;
	for (r=0; r< numRows; r++) {
		for (c=0; c< numCols; c++) {
			int16_t dstIndex = MatrixIndex(r, c, numRows);
			int16_t srcIndex = MatrixIndex(c, r, numCols);
			dstM[dstIndex] = srcM[srcIndex];
		}
	}
	return dstM;
}


fractional* VectorCopy (
						int numElems,
						fractional* dstV,
						fractional* srcV
						)
{
	int16_t i;
	for (i=0; i< numElems; i++) {
		dstV[i] = srcV[i];
	}
	return dstV;
}

fractional* VectorAdd (          /* Vector addition */
					   /* dstV[elem] =                 */
					   /*    = srcV1[elem] + srcV2[elem] */
					   /* (in place capable) */
					   /* (with itself capable) */
					   int16_t numElems,                        /* number elements in srcV[1,2] (N) */
					   fractional* dstV,                    /* ptr to destination vector */
					   fractional* srcV1,                   /* ptr to source vector one */
					   fractional* srcV2                    /* ptr to source vector two */
					   
					   /* dstV returned */
					   )
{
	int16_t i;
	for (i=0; i< numElems; i++) {
		dstV[i] = fl2fr(fr2fl(srcV1[i]) + fr2fl(srcV2[i]));
	}
	return dstV;
}


fractional* VectorSubtract (          /* Vector subtract */
							/* dstV[elem] =                 */
							/*    = srcV1[elem] - srcV2[elem] */
							/* (in place capable) */
							/* (with itself capable) */
							int16_t numElems,                        /* number elements in srcV[1,2] (N) */
							fractional* dstV,                    /* ptr to destination vector */
							fractional* srcV1,                   /* ptr to source vector one */
							fractional* srcV2                    /* ptr to source vector two */
							
							/* dstV returned */
							)
{
	int16_t i;
	for (i=0; i< numElems; i++) {
		dstV[i] = fl2fr(fr2fl(srcV1[i]) - fr2fl(srcV2[i]));
	}
	return dstV;
}


fractional* VectorMultiply (     /* Vector elem-to-elem multiply */
							/* dstV[elem] =                 */
							/*    = srcV1[elem] * srcV2[elem] */
							/* (in place capable) */
							/* (with itself capable) */
							int16_t numElems,                        /* number elements in srcV[1,2] (N) */
							fractional* dstV,                    /* ptr to destination vector */
							fractional* srcV1,                   /* ptr to source vector one */
							fractional* srcV2                    /* ptr to source vector two */
							
							/* dstV returned */
							)
{
	int16_t i;
	for (i=0; i< numElems; i++) {
		dstV[i] = fl2fr(fr2fl(srcV1[i]) * fr2fl(srcV2[i]));
	}
	return dstV;
}


fractional VectorDotProduct (    /* Vector dot product */
							 /* dotVal =                     */
							 /*    = sum(srcV1[elem]*srcV2[elem]) */
							 /* (with itself capable) */
							 int16_t numElems,                        /* number elements in srcV[1,2] (N) */
							 fractional* srcV1,                   /* ptr to source vector one */
							 fractional* srcV2                    /* ptr to source vector two */
							 
							 /* dot product value returned */
							 )
{
	fractional sum = 0;
	int16_t i;
	for (i=0; i< numElems; i++) {
		sum += fl2fr(fr2fl(srcV1[i]) * fr2fl(srcV2[i]));
	}
	return sum;
}


fractional VectorPower (         /* Vector power */
						/* powVal =                     */
						/*    = sum(srcV[elem]^2)       */
						int16_t numElems,                        /* number elements in srcV (N) */
						fractional* srcV                     /* ptr to source vector one */
						
						/* power value returned */
						)
{
	fractional sum = 0;
	int16_t i;
	for (i=0; i< numElems; i++) {
		sum += fl2fr(fr2fl(srcV[i]) * fr2fl(srcV[i]));
	}
	return sum;
}


fractional* VectorScale (        /* Vector scale */
						 /* dstV[elem] = sclVal*srcV[elem] */
						 /* (in place capable) */
						 int16_t numElems,                        /* number elements in srcV (N) */
						 fractional* dstV,                    /* ptr to destination vector */
						 fractional* srcV,                    /* ptr to source vector */
						 fractional sclVal                    /* scale value (Q.15 fractional) */
						 
						 /* dstV returned */
						 )
{
	int16_t i;
	for (i=0; i< numElems; i++) {
		dstV[i] = fl2fr(fr2fl(srcV[i]) * fr2fl(sclVal));
	}
	return dstV;
}

