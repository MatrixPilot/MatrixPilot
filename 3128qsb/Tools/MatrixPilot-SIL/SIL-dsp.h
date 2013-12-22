//
//  SIL-dsp.h
//  MatrixPilot-SIL
//
//  Created by Ben Levitt on 2/4/13.
//  Copyright (c) 2013 MatrixPilot. All rights reserved.
//

#ifndef MatrixPilot_SIL_SIL_dsp_h
#define MatrixPilot_SIL_SIL_dsp_h

#include <stdint.h>

typedef int16_t fractional;

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
					   );

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
							);


fractional* MatrixTranspose (
							int numRows,
							int numCols,
							fractional* dstM,
							fractional* srcM
							);


fractional* VectorCopy (
						int numElems,
						fractional* dstV,
						fractional* srcV
						);


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
					   );

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
					   );

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
							);

fractional VectorDotProduct (    /* Vector dot product */
							 /* dotVal =                     */
							 /*    = sum(srcV1[elem]*srcV2[elem]) */
							 /* (with itself capable) */
							 int16_t numElems,                        /* number elements in srcV[1,2] (N) */
							 fractional* srcV1,                   /* ptr to source vector one */
							 fractional* srcV2                    /* ptr to source vector two */
							 
							 /* dot product value returned */
							 );

fractional VectorPower (         /* Vector power */
						/* powVal =                     */
						/*    = sum(srcV[elem]^2)       */
						int16_t numElems,                        /* number elements in srcV (N) */
						fractional* srcV                     /* ptr to source vector one */
						
						/* power value returned */
						);

fractional* VectorScale (        /* Vector scale */
						 /* dstV[elem] = sclVal*srcV[elem] */
						 /* (in place capable) */
						 int16_t numElems,                        /* number elements in srcV (N) */
						 fractional* dstV,                    /* ptr to destination vector */
						 fractional* srcV,                    /* ptr to source vector */
						 fractional sclVal                    /* scale value (Q.15 fractional) */
						 
						 /* dstV returned */
						 );

#endif
