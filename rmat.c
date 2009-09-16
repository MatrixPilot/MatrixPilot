

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
fractional errorTotal[] = { 0 , 0 , 0 } ;

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
// library, so interrupts should be temporarily suspended
void interruptsOff(void)
{
	__asm__("DISI #1000");
	return ;
}
void interruptsOn(void)
{
	DISICNT = 0 ;
	return ;
}

void read_gyros()
//	fetch the gyro signals and subtract the baseline offset, 
//	and adjust for variations in supply voltage
{
	int vref_adj ;
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

void read_accel()
{
	gplane[0] =   ( xaccel.value>>1 ) - ( xaccel.offset>>1 ) ;
	gplane[1] =   ( yaccel.value>>1 ) - ( yaccel.offset>>1 ) ;
	gplane[2] =   ( zaccel.value>>1 ) - ( zaccel.offset>>1 ) ;
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
	else if ( working._.W1 < - CENTRIFSAT )
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
//	gplane[1]=gplane[1] ;
	gplane[2]=gplane[2]+ omegaSOG( omegaAccum[0] , (unsigned int) sog_gps.BB ) ;

//	gplane[0]=gplane[0]- omegaSOG( omegaAccum[2] , (unsigned int) velocity_magnitude ) ;
	gplane[1]=gplane[1]+ ACCELSCALE*forward_acceleration ;
//	gplane[2]=gplane[2]+ omegaSOG( omegaAccum[0] , (unsigned int) velocity_magnitude ) ;

	return ;
}

//	The update algorithm!!
void rupdate(void)
//	This is the key routine. It performs a small rotation
//	on the direction cosine matrix, based on the gyro vector and correction.
//	It uses vector and matrix routines furnished by Microchip.
{
	interruptsOff();
	VectorAdd( 3 , omegaAccum , omegagyro , omegacorrI ) ;
	VectorAdd( 3 , omega , omegaAccum , omegacorrP ) ;
	//	scale by the integration factor:
	VectorScale( 3 , theta , omega , ggain ) ;
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
	interruptsOn() ;
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
	interruptsOff() ;
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
	norm = VectorPower( 3 , &rbuff[0] ) ;
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
	interruptsOn() ;
	return ;
}

void roll_pitch_drift()
{
	interruptsOff() ;
	VectorCross( errorRP , gplane , &rmat[6] ) ;
	interruptsOn() ;
	return ;
}

void yaw_drift()
{
	//	although yaw correction is done in horizontal plane,
	//	this is done in 3 dimensions, just in case we change our minds later
	//	form the horizontal direction over ground based on rmat
	interruptsOff() ;
	if (flags._.yaw_req )
	{
		//	vector cross product to get the rotation error in ground frame
		VectorCross( errorYawground , dirovergndHRmat , dirovergndHGPS ) ;
		//	convert to plane frame:
		//	*** Note: this accomplishes multiplication rmat transpose times errorYawground!!
		MatrixMultiply( 1 , 3 , 3 , errorYawplane , errorYawground , rmat ) ;

		flags._.yaw_req = 0 ;
	}
	interruptsOn();
	return ;
}

void PI_feedback(void)
{
	fractional errorRPScaled[3] ;
	interruptsOff();
	VectorAdd( 3 , errorTotal , errorRP , errorYawplane ) ;

	VectorScale( 3 , omegacorrP , errorYawplane , KPYAW ) ;
	VectorScale( 3 , errorRPScaled , errorRP , KPROLLPITCH ) ;
	VectorAdd( 3 , omegacorrP , omegacorrP , errorRPScaled ) ;

	CorrectionIntegral[0].WW += ( __builtin_mulss( errorRP[0] , KIROLLPITCH )>>3) ;
	CorrectionIntegral[1].WW += ( __builtin_mulss( errorRP[1] , KIROLLPITCH )>>3) ;
	CorrectionIntegral[2].WW += ( __builtin_mulss( errorRP[2] , KIROLLPITCH )>>3) ;

	CorrectionIntegral[0].WW += ( __builtin_mulss( errorYawplane[0] , KIYAW )>>3) ;
	CorrectionIntegral[1].WW += ( __builtin_mulss( errorYawplane[1] , KIYAW )>>3) ;
	CorrectionIntegral[2].WW += ( __builtin_mulss( errorYawplane[2] , KIYAW )>>3) ;

	omegacorrI[0] = CorrectionIntegral[0]._.W1>>3 ;
	omegacorrI[1] = CorrectionIntegral[1]._.W1>>3 ;
	omegacorrI[2] = CorrectionIntegral[2]._.W1>>3 ;
	
	interruptsOn();
}

extern signed char computed_cog ;

void output_matrix(void)
//	This routine makes the direction cosine matrix evident
//	by setting the three servos to the three values in the
//	matrix.
{
	union longww accum ;
	accum.WW = __builtin_mulss( rmat[6] , 4000 ) ;
	PDC1 = 3000 + accum._.W1 ;
	accum.WW = __builtin_mulss( rmat[7] , 4000 ) ;
	PDC2 = 3000 + accum._.W1 ;
	accum.WW = __builtin_mulss( rmat[4] , 4000 ) ;
	PDC3 = 3000 + accum._.W1 ;

	return ;
}

void imu(void)
//	Read the gyros and accelerometers, 
//	update the matrix, renormalize it, 
//	adjust for roll and pitch drift,
//	and send it to the servos.
{
	read_gyros() ;
	read_accel() ;
	adj_accel() ;
	rupdate() ;
	normalize() ;
	roll_pitch_drift() ;
	yaw_drift() ;
	PI_feedback() ;
//	output_matrix() ;	
	return ;
}

