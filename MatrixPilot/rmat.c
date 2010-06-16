// This file is part of MatrixPilot.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009, 2010 MatrixPilot Team
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


//		These are the routines for maintaining a direction cosine matrix
//		that can be used to transform vectors between the earth and plane
//		coordinate systems. The 9 direction cosines in the matrix completely
//		define the orientation of the plane with respect to the earth.
//		The inverse of the matrix is equal to its transpose. This defines
//		the so-called orthogonality conditions, which impose 6 constraints on
//		the 9 elements of the matrix.


#include "p30f4011.h"
#include "defines.h"
#include "definesRmat.h"
#include "magnetometerOptions.h"


//	All numbers are stored in 2.14 format.
//	Vector and matrix libraries work in 1.15 format.
//	This combination allows values of matrix elements between -2 and +2.
//	Multiplication produces results scaled by 1/2.

#define GGAIN SCALEGYRO*6*(RMAX*0.025)		//	integration multiplier for gyros 15mv/degree/sec
fractional ggain = GGAIN ;

//Paul's gains:
#define KPROLLPITCH 256*10
#define KIROLLPITCH 256*2

#define KPYAW 256*4
#define KIYAW 32

#define GYROSAT 15000
// threshold at which gyros may be saturated

//	rmat is the matrix of direction cosines relating
//	the body and earth coordinate systems.
//	The columns of rmat are the axis vectors of the plane,
//	as measured in the earth reference frame.
//	rmat is initialized to the identity matrix in 2.14 fractional format
fractional rmat[] = { RMAX , 0 , 0 , 0 , RMAX , 0 , 0 , 0 , RMAX } ;

//	rup is the rotational update matrix.
//	At each time step, the new rmat is equal to the old one, multiplied by rup.
fractional rup[] = { RMAX , 0 , 0 , 0 , RMAX , 0 , 0 , 0 , RMAX } ;

//	gyro rotation vector:
fractional omegagyro[] = { 0 , 0 , 0 } ;
fractional omega[] = { 0 , 0 , 0 } ;

//	gyro correction vectors:
fractional omegacorrP[] = { 0 , 0 , 0 } ;
fractional omegacorrI[] = { 0 , 0 , 0 } ;

//	correction vector integrators ;
union longww CorrectionIntegral[] =  { { 0 } , { 0 } ,  { 0 } } ;

//	accumulator for computing adjusted omega:
fractional omegaAccum[] = { 0 , 0 , 0 } ;

//	gravity, as measured in plane coordinate system
fractional gplane[] = { 0 , 0 , GRAVITY } ;

//	horizontal velocity over ground, as measured by GPS (Vz = 0 )
fractional dirovergndHGPS[] = { 0 , RMAX/2 , 0 } ;

//	horizontal direction over ground, as indicated by Rmatrix
fractional dirovergndHRmat[] = { 0 , RMAX/2 , 0 } ;

//	rotation angle equal to omega times integration factor:
fractional theta[] = { 0 , 0 , 0 } ;

//	matrix buffer:
fractional rbuff[] = { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 } ;

//	vector buffer
fractional errorRP[] = { 0 , 0 , 0 } ;
fractional errorYawground[] = { 0 , 0 , 0 } ;
fractional errorYawplane[]  = { 0 , 0 , 0 } ;

//	measure of error in orthogonality, used for debugging purposes:
fractional error = 0 ;

//	Implement the cross product. *dest = *src1X*src2 ;
void VectorCross( fractional * dest , fractional * src1 , fractional * src2 )
{
	union longww crossaccum ;
	crossaccum.WW = __builtin_mulss( src1[1] , src2[2] ) ;
	crossaccum.WW -= __builtin_mulss( src1[2] , src2[1] ) ;
	crossaccum.WW *= 4 ;
	dest[0] = crossaccum._.W1 ;
	crossaccum.WW = __builtin_mulss( src1[2] , src2[0] ) ;
	crossaccum.WW -= __builtin_mulss( src1[0] , src2[2] ) ;
	crossaccum.WW *= 4 ;
	dest[1] = crossaccum._.W1 ;
	crossaccum.WW = __builtin_mulss( src1[0] , src2[1] ) ;
	crossaccum.WW -= __builtin_mulss( src1[1] , src2[0] ) ;
	crossaccum.WW *= 4 ;
	dest[2] = crossaccum._.W1 ;
	return ;
}

// the compiler does not respect the resource used by the Microchip math
// library, so interrupts need to save and restore extended state, and
// reset CORCON if firing in the middle of a math lib call.
void setDSPLibInUse(boolean inUse)
{
	needSaveExtendedState = inUse ;
	return ;
}

int vref_adj ;

void read_gyros()
//	fetch the gyro signals and subtract the baseline offset, 
//	and adjust for variations in supply voltage
{
	int gx , gy , gz ;
#ifdef VREF
	vref_adj = (vref.offset>>1) - (vref.value>>1) ;
#else
	vref_adj = 0 ;
#endif

	gx = omegagyro[0] = XSIGN ((xrate.value>>1) - (xrate.offset>>1) + vref_adj) ;
	gy = omegagyro[1] = YSIGN ((yrate.value>>1) - (yrate.offset>>1) + vref_adj) ;
	gz = omegagyro[2] = ZSIGN ((zrate.value>>1) - (zrate.offset>>1) + vref_adj) ;
	return ;
}

//  acceleration, as measured in GPS earth coordinate system
fractional accelEarth[] = { 0 , 0 , 0 } ;

void read_accel()
{
	setDSPLibInUse(true) ;

	gplane[0] =   ( xaccel.value>>1 ) - ( xaccel.offset>>1 ) ;
	gplane[1] =   ( yaccel.value>>1 ) - ( yaccel.offset>>1 ) ;
	gplane[2] =   ( zaccel.value>>1 ) - ( zaccel.offset>>1 ) ;

	accelEarth[0] =  VectorDotProduct( 3 , &rmat[0] , gplane )<<1;
	accelEarth[1] = - VectorDotProduct( 3 , &rmat[3] , gplane )<<1;
	accelEarth[2] = -((int)GRAVITY) + (VectorDotProduct( 3 , &rmat[6] , gplane )<<1);  

	setDSPLibInUse(false) ;

	return ;
}


#define ACCEL2DELTAV ((.025*GRAVITYM*4.0*RMAX)/GRAVITY)
#define VELOCITY2LOCATION (0.025*.01*4.0*RMAX)

#define VELTAU 5.0

#define TIMESTEP 0.025
#define MAX16 (4.0*RMAX)

#define KPVELREAL (2.0/VELTAU)
#define KIVELREAL (1.0/(VELTAU*VELTAU))

#define KPVELOCITY (TIMESTEP*MAX16*16.0*KPVELREAL)
#define KIVELOCITY (TIMESTEP*TIMESTEP*MAX16*256.0*KIVELREAL)

#define KPLOCATION (TIMESTEP*MAX16*8.0*KPVELREAL)
#define KILOCATION (TIMESTEP*TIMESTEP*MAX16*128.0*KIVELREAL)

//	velocity, as estimated by the IMU
union longww IMUvelocityx =  { 0 }  ;
union longww IMUvelocityy =  { 0 }  ;
union longww IMUvelocityz =  { 0 }  ;

//	velocity error in the IMU feedback loop 
fractional velocityErrorBody[] = { 0 , 0 , 0 } ;
fractional locationErrorEarth[] = { 0 , 0 , 0 } ;

//	IMU velocity correction vectors:
int velocityCorrP[] = { 0 , 0 , 0 } ;
int velocityCorrI[] = { 0 , 0 , 0 } ;

//	location, as estimated by the IMU
union longww IMUlocationx =  { 0 }  ;
union longww IMUlocationy =  { 0 }  ;
union longww IMUlocationz =  { 0 }  ;


//	IMU location correction vectors:
int locationCorrP[] = { 0 , 0 , 0 } ;
int locationCorrI[] = { 0 , 0 , 0 } ;

//	correction velocity vector integrators ;
union longww velocityCorrectionIntegral[] =  { { 0 } , { 0 } ,  { 0 } } ;

//	correction position vector integrators ;
union longww locationCorrectionIntegral[] =  { { 0 } , { 0 } ,  { 0 } } ;

void dead_reckon_velocity()
{
	union longww accumulator ;
	fractional velocityErrorEarth[3] ;
	fractional CorrTotal[3] ;

	CorrTotal[0] = (velocityCorrP[0] + velocityCorrI[0]) ;
	CorrTotal[1] = (velocityCorrP[1] + velocityCorrI[1]) ;
	CorrTotal[2] = (velocityCorrP[2] + velocityCorrI[2]) ;

	setDSPLibInUse(true) ;

	accumulator.WW = __builtin_mulss( ((int)(ACCEL2DELTAV)) ,  accelEarth[0] ) ;
	IMUvelocityx.WW += accumulator.WW
				 + (((long)VectorDotProduct( 3 , CorrTotal , &rmat[0] ))<<12 ) ; // Dotgain = 1/2

	accumulator.WW = __builtin_mulss( ((int)(ACCEL2DELTAV)) ,  accelEarth[1] ) ;
	IMUvelocityy.WW += accumulator.WW
				 + (((long)VectorDotProduct( 3 , CorrTotal , &rmat[3] ))<<12 ) ; // Dotgain = 1/2

	accumulator.WW = __builtin_mulss( ((int)(ACCEL2DELTAV)) ,  accelEarth[2] ) ;
	IMUvelocityz.WW += accumulator.WW 
				+ (((long)VectorDotProduct( 3 , CorrTotal , &rmat[6] ))<<12 ) ; // Dotgain = 1/2

	if ( gps_nav_valid() && ( cos_lat != 0)  )  // started up and running
	{
		velocityErrorEarth[0] = (GPSvelocity.x - IMUvelocityx._.W1) ;
		velocityErrorEarth[1] = (GPSvelocity.y - IMUvelocityy._.W1) ;
		velocityErrorEarth[2] = (GPSvelocity.z - IMUvelocityz._.W1) ;
	}
	else if ( cos_lat == 0 )  // waiting to start up
	{
		velocityErrorEarth[0] = ( - IMUvelocityx._.W1) ;
		velocityErrorEarth[1] = ( - IMUvelocityy._.W1) ;
		velocityErrorEarth[2] = ( - IMUvelocityz._.W1) ;
	}
	else  // running, but GPS is lost, so go dead reckoning for a while
	{
		velocityErrorEarth[0] = 0 ;
		velocityErrorEarth[1] = 0 ;
		velocityErrorEarth[2] = 0 ;
	}

	MatrixMultiply( 1 , 3 , 3 , velocityErrorBody , velocityErrorEarth , rmat ) ;

	setDSPLibInUse(false) ;

	CorrTotal[0] = (locationCorrP[0] + locationCorrI[0]) ;
	CorrTotal[1] = (locationCorrP[1] + locationCorrI[1]) ;
	CorrTotal[2] = (locationCorrP[2] + locationCorrI[2]) ;

	accumulator.WW = __builtin_mulss( ((int)(VELOCITY2LOCATION)) ,  IMUvelocityx._.W1 ) ;
	IMUlocationx.WW += accumulator.WW + (((long)CorrTotal[0] )<<8)  ; 

	accumulator.WW = __builtin_mulss( ((int)(VELOCITY2LOCATION)) ,  IMUvelocityy._.W1 ) ;
	IMUlocationy.WW += accumulator.WW + (((long)CorrTotal[1] )<<8)  ; 

	accumulator.WW = __builtin_mulss( ((int)(VELOCITY2LOCATION)) ,  IMUvelocityz._.W1 ) ;
	IMUlocationz.WW += accumulator.WW + (((long)CorrTotal[2] )<<8)  ; 

	if ( gps_nav_valid() && ( cos_lat != 0)  )  // started up and running
	{
		locationErrorEarth[0] = ((GPSlocation.x<<16) - IMUlocationx.WW)>>12 ;
		locationErrorEarth[1] = ((GPSlocation.y<<16) - IMUlocationy.WW)>>12 ;
		locationErrorEarth[2] = ((((long)GPSlocation.z)<<16) - IMUlocationz.WW)>>12 ;	
	}
	else if ( cos_lat == 0 )  // waiting to start up
	{
		locationErrorEarth[0] = ( - IMUlocationx.WW)>>12 ;
		locationErrorEarth[1] = ( - IMUlocationy.WW)>>12 ;
		locationErrorEarth[2] = ( - IMUlocationz.WW)>>12 ;	
	}
	else  // running, but GPS is lost, so go dead reckoning for a while
	{
		locationErrorEarth[0] = 0 ;
		locationErrorEarth[1] = 0 ;
		locationErrorEarth[2] = 0 ;	
	}

	return ;
}

//	multiplies omega times speed, and scales appropriately
int omegaSOG ( int omega , unsigned int speed  )
{
	union longww working ;
	speed = speed>>3 ;
	working.WW = __builtin_mulsu( omega , speed ) ;
	if ( ((int)working._.W1 )> ((int)CENTRIFSAT) )
	{
		return RMAX ;
	}
	else if ( ((int)working._.W1) < ((int)-CENTRIFSAT) )
	{
		return - RMAX ;
	}
	else
	{
		working.WW = working.WW>>5 ;
		working.WW = __builtin_mulsu( working._.W0 , CENTRISCALE ) ;
		working.WW = working.WW<<5 ;
		return working._.W1 ;
	}
}

void adj_accel()
{
	gplane[0]=gplane[0]- omegaSOG( omegaAccum[2] , (unsigned int) sog_gps.BB ) ;
	gplane[2]=gplane[2]+ omegaSOG( omegaAccum[0] , (unsigned int) sog_gps.BB ) ;	
	gplane[1]=gplane[1]+ ((int)(ACCELSCALE))*forward_acceleration ;	
	return ;
}

//	The update algorithm!!
void rupdate(void)
//	This is the key routine. It performs a small rotation
//	on the direction cosine matrix, based on the gyro vector and correction.
//	It uses vector and matrix routines furnished by Microchip.
{
	setDSPLibInUse(true) ;
	VectorAdd( 3 , omegaAccum , omegagyro , omegacorrI ) ;
	VectorAdd( 3 , omega , omegaAccum , omegacorrP ) ;
	//	scale by the integration factor:
	VectorScale( 3 , theta , omega , ggain ) ; // Scalegain of 2
	//	construct the off-diagonal elements of the update matrix:
	rup[1] = -theta[2] ;
	rup[2] =  theta[1] ;
	rup[3] =  theta[2] ;
	rup[5] = -theta[0] ;
	rup[6] = -theta[1] ;
	rup[7] =  theta[0] ;
	//	matrix multiply the rmatrix by the update matrix
	MatrixMultiply( 3 , 3 , 3 , rbuff , rmat , rup ) ;
	//	multiply by 2 and copy back from rbuff to rmat:
	MatrixAdd( 3 , 3 , rmat , rbuff , rbuff ) ; 
	setDSPLibInUse(false) ;
	return ;
}

//	normalization algorithm:
void normalize(void)
//	This is the routine that maintains the orthogonality of the
//	direction cosine matrix, which is expressed by the identity
//	relationship that the cosine matrix multiplied by its
//	transpose should equal the identity matrix.
//	Small adjustments are made at each time step to assure orthogonality.
{
	fractional norm ; // actual magnitude
	fractional renorm ;	// renormalization factor
	setDSPLibInUse(true) ;
	//	compute -1/2 of the dot product between rows 1 and 2
	error =  - VectorDotProduct( 3 , &rmat[0] , &rmat[3] ) ; // note, 1/2 is built into 2.14
	//	scale rows 1 and 2 by the error
	VectorScale( 3 , &rbuff[0] , &rmat[3] , error ) ; 
	VectorScale( 3 , &rbuff[3] , &rmat[0] , error ) ; 
	//	update the first 2 rows to make them closer to orthogonal:
	VectorAdd( 3 , &rbuff[0] , &rbuff[0] , &rmat[0] ) ;
	VectorAdd( 3 , &rbuff[3] , &rbuff[3] , &rmat[3] ) ;
	//	use the cross product of the first 2 rows to get the 3rd row
	VectorCross( &rbuff[6] , &rbuff[0] , &rbuff[3] ) ;


	//	Use a Taylor's expansion for 1/sqrt(X*X) to avoid division in the renormalization
	//	rescale row1
	norm = VectorPower( 3 , &rbuff[0] ) ; // scale gain = .5
	renorm = RMAX15 - norm ;
	VectorScale( 3 , &rbuff[0] , &rbuff[0] , renorm ) ; 
	VectorAdd( 3 , &rmat[0] , &rbuff[0] , &rbuff[0] ) ;
	//	rescale row2
	norm = VectorPower( 3 , &rbuff[3] ) ;
	renorm = RMAX15 - norm ;
	VectorScale( 3 , &rbuff[3] , &rbuff[3] , renorm ) ; 
	VectorAdd( 3 , &rmat[3] , &rbuff[3] , &rbuff[3] ) ;
	//	rescale row3
	norm = VectorPower( 3 , &rbuff[6] ) ;
	renorm = RMAX15 - norm ;
	VectorScale( 3 , &rbuff[6] , &rbuff[6] , renorm ) ; 
	VectorAdd( 3 , &rmat[6] , &rbuff[6] , &rbuff[6] ) ;
	setDSPLibInUse(false) ;
	return ;
}

void roll_pitch_drift()
{
	setDSPLibInUse(true) ;
	VectorCross( errorRP , gplane , &rmat[6] ) ;
	setDSPLibInUse(false) ;
	return ;
}

void yaw_drift()
{
	//	although yaw correction is done in horizontal plane,
	//	this is done in 3 dimensions, just in case we change our minds later
	//	form the horizontal direction over ground based on rmat
	if (flags._.yaw_req )
	{
		if ( velocity_magnitude > GPS_SPEED_MIN )
		{
			setDSPLibInUse(true) ;
			//	vector cross product to get the rotation error in ground frame
			VectorCross( errorYawground , dirovergndHRmat , dirovergndHGPS ) ;
			//	convert to plane frame:
			//	*** Note: this accomplishes multiplication rmat transpose times errorYawground!!
			MatrixMultiply( 1 , 3 , 3 , errorYawplane , errorYawground , rmat ) ;
			setDSPLibInUse(false) ;
		}
		else
		{
			errorYawplane[0] = errorYawplane[1] = errorYawplane[2] = 0 ;
		}

		flags._.yaw_req = 0 ;
	}
	return ;
}

fractional magFieldEarth[3] ;

extern fractional magFieldBody[3] ;
extern fractional magOffset[3] ;

fractional magFieldEarthPrevious[3] ;
fractional magFieldBodyPrevious[3] ;

fractional rmatPrevious[9] ;

int offsetDelta[3] ;

extern fractional declinationVector[2] ;

void align_rmat_to_mag(void)
{
	unsigned char theta ;
	struct relative2D initialBodyField ;
	int costheta ;
	int sintheta ;
	initialBodyField.x = magFieldBody[0] ;
	initialBodyField.y = magFieldBody[1] ;
	theta = rect_to_polar( &initialBodyField ) -64 - DECLINATIONANGLE ;
	costheta = cosine(theta) ;
	sintheta = sine(theta) ;
	rmat[0] = rmat[4] = costheta ;
	rmat[1] = sintheta ;
	rmat[3] = - sintheta ;
	return ;
}

void mag_drift()
{
	int mag_error ;
	int vector_index ;
	fractional rmatTransposeMagField[3] ;
	fractional offsetSum[3] ;
	fractional deltaMagField[3] ;
	if ( flags._.mag_drift_req )
	{
		setDSPLibInUse(true) ;

		if ( flags._.first_mag_reading == 1 )
		{
			align_rmat_to_mag() ;
		}

		magFieldEarth[0] = VectorDotProduct( 3 , &rmat[0] , magFieldBody )<<1 ;
		magFieldEarth[1] = VectorDotProduct( 3 , &rmat[3] , magFieldBody )<<1 ;
		magFieldEarth[2] = VectorDotProduct( 3 , &rmat[6] , magFieldBody )<<1 ;

		mag_error = 100*VectorDotProduct( 2 , magFieldEarth , declinationVector ) ; //Dotgain = 1/2
		VectorScale( 3 , errorYawplane , &rmat[6] , mag_error ) ;  //Scalegain = 1/2

		VectorAdd( 3 , offsetSum , magFieldBody , magFieldBodyPrevious ) ;
		for ( vector_index = 0 ; vector_index < 3 ; vector_index++ )
		{
			offsetSum[vector_index] >>= 1 ;
		}

		MatrixMultiply( 1 , 3 , 3 , rmatTransposeMagField , magFieldEarthPrevious , rmat ) ;
		VectorSubtract( 3 , offsetSum , offsetSum , rmatTransposeMagField ) ;

		MatrixMultiply( 1 , 3 , 3 , rmatTransposeMagField , magFieldEarth , rmatPrevious ) ;
		VectorSubtract( 3 , offsetSum , offsetSum , rmatTransposeMagField ) ;

		for ( vector_index = 0 ; vector_index < 3 ; vector_index++ )
		{
			int adjustment ;
			adjustment = offsetSum[vector_index] ;
			if ( abs( adjustment ) < 3 )
			{
				offsetSum[vector_index] = 0 ;
				adjustment = 0 ;
			}
			offsetDelta[vector_index] = adjustment ;
		}

		if ( flags._.first_mag_reading == 0 )
		{
			VectorAdd ( 3 , magOffset , magOffset , offsetSum ) ;
		}
		else
		{
			flags._.first_mag_reading = 0 ;
		}

		VectorCopy ( 3 , magFieldEarthPrevious , magFieldEarth ) ;
		VectorCopy ( 3 , magFieldBodyPrevious , magFieldBody ) ;
		VectorCopy ( 9 , rmatPrevious , rmat ) ;

		setDSPLibInUse(false) ;
		flags._.mag_drift_req = 0 ;
	}
	return ;
}


void PI_feedback(void)
{
	fractional errorRPScaled[3] ;
	
	setDSPLibInUse(true) ;
	
	VectorScale( 3 , omegacorrP , errorYawplane , KPYAW ) ; // scale gain = 2
	VectorScale( 3 , errorRPScaled , errorRP , KPROLLPITCH ) ; // scale gain = 2
	VectorAdd( 3 , omegacorrP , omegacorrP , errorRPScaled ) ;

	VectorScale( 3 , velocityCorrP , velocityErrorBody , ((int)(KPVELOCITY)) ) ; // scales by 2
	VectorScale( 3 , locationCorrP , locationErrorEarth , ((int)(KPLOCATION)) ) ; // scales by 2

	setDSPLibInUse(false) ;
	
	CorrectionIntegral[0].WW += ( __builtin_mulss( errorRP[0] , KIROLLPITCH )>>3) ;
	CorrectionIntegral[1].WW += ( __builtin_mulss( errorRP[1] , KIROLLPITCH )>>3) ;
	CorrectionIntegral[2].WW += ( __builtin_mulss( errorRP[2] , KIROLLPITCH )>>3) ;

	CorrectionIntegral[0].WW += ( __builtin_mulss( errorYawplane[0] , KIYAW )>>3) ;
	CorrectionIntegral[1].WW += ( __builtin_mulss( errorYawplane[1] , KIYAW )>>3) ;
	CorrectionIntegral[2].WW += ( __builtin_mulss( errorYawplane[2] , KIYAW )>>3) ;

	velocityCorrectionIntegral[0].WW += ( __builtin_mulss( velocityErrorBody[0] , ((int)(KIVELOCITY)) )) ;
	velocityCorrectionIntegral[1].WW += ( __builtin_mulss( velocityErrorBody[1] , ((int)(KIVELOCITY)) )) ;
	velocityCorrectionIntegral[2].WW += ( __builtin_mulss( velocityErrorBody[2] , ((int)(KIVELOCITY)) )) ;

	locationCorrectionIntegral[0].WW += ( __builtin_mulss( locationErrorEarth[0] , ((int)(KILOCATION)) )) ;
	locationCorrectionIntegral[1].WW += ( __builtin_mulss( locationErrorEarth[1] , ((int)(KILOCATION)) )) ;
	locationCorrectionIntegral[2].WW += ( __builtin_mulss( locationErrorEarth[2] , ((int)(KILOCATION)) )) ;

	omegacorrI[0] = CorrectionIntegral[0]._.W1>>3 ;
	omegacorrI[1] = CorrectionIntegral[1]._.W1>>3 ;
	omegacorrI[2] = CorrectionIntegral[2]._.W1>>3 ;

	velocityCorrI[0] = (velocityCorrectionIntegral[0]._.W1)>>3 ;
	velocityCorrI[1] = (velocityCorrectionIntegral[1]._.W1)>>3 ;
	velocityCorrI[2] = (velocityCorrectionIntegral[2]._.W1)>>3 ;

	locationCorrI[0] = (locationCorrectionIntegral[0]._.W1)>>3 ;
	locationCorrI[1] = (locationCorrectionIntegral[1]._.W1)>>3 ;
	locationCorrI[2] = (locationCorrectionIntegral[2]._.W1)>>3 ;

	return ;
}

/*
void output_matrix(void)
//	This routine makes the direction cosine matrix evident
//	by setting the three servos to the three values in the
//	matrix.
{
	union longww accum ;
	accum.WW = __builtin_mulss( rmat[6] , 4000 ) ;
//	PDC1 = 3000 + accum._.W1 ;
//	accum.WW = __builtin_mulss( rmat[7] , 4000 ) ;
	accum.WW = __builtin_mulss( rmat[3] , 4000 ) ;
	PDC2 = pulsesat( 3000 + accum._.W1 + waggle ) ;
	accum.WW = __builtin_mulss( rmat[4] , 4000 ) ;
	PDC3 = pulsesat( 3000 + accum._.W1 + waggle ) ;
	return ;
}


void output_IMUvelocity(void)
{
//	PDC1 = pulsesat( IMUvelocityx._.W1 + waggle + 3000 ) ;
//	PDC2 = pulsesat( IMUvelocityy._.W1 + waggle + 3000 ) ;
//	PDC3 = pulsesat( IMUvelocityz._.W1 + waggle + 3000 ) ;

//	PDC1 = pulsesat( accelEarth[0] + 3000 ) ;
//	PDC2 = pulsesat( accelEarth[1] + 3000 ) ;
//	PDC3 = pulsesat( accelEarth[2] + 3000 ) ;

	return ;
}
*/


void imu(void)
//	Read the gyros and accelerometers, 
//	update the matrix, renormalize it, 
//	adjust for roll and pitch drift,
//	and send it to the servos.
{
	read_gyros() ;
	read_accel() ;
	dead_reckon_velocity() ;
	adj_accel() ;
	rupdate() ;
	normalize() ;
	roll_pitch_drift() ;
	if ( (MAG_YAW_DRIFT == 1) && ( magMessage == 7 ) )
	{
		mag_drift() ;
	}
	else
	{
		yaw_drift() ;
	}

	PI_feedback() ;
	processwaypoints() ;
//	output_matrix() ;
	return ;
}

