

//		These are the routines for maintaining a direction cosine matrix
//		that can be used to transform vectors between the earth and plane
//		coordinate systems. The 9 direction cosines in the matrix completely
//		define the orientation of the plane with respect to the earth.
//		The inverse of the matrix is equal to its transpose. This defines
//		the so-called orthogonality conditions, which impose 6 constraints on
//		the 9 elements of the matrix.

//		This is a work in progress. Much of it is done, but there is more that
//		still needs to be done.

//		Here is what is complete:
//		1. 	The basic integration implementation to use gyro information to
//			update the matrix for a small rotation.
//		2.	A renormalization routine that satisfies the orthogonality conditions.
//		3.	Roll-pitch drift cancellation based on accelerometer data.

//		Here is what is left to do:
//		4.	Adjustment of acceleration for centrifugal effects.
//		5.	Yaw drift cancellation based on GPS data.
//		6.	Adaptive adjustment of gyro gains using a regression analysis of drift corrections.

//		W. Premerlani, 2/21/2009.

#include "p30f4011.h"
#include "definesRmat.h"
#include "defines.h"

//	All numbers are stored in 2.14 format.
//	Vector and matrix libraries work in 1.15 format.
//	This combination allows values of matrix elements between -2 and +2.
//	Multiplication produces results scaled by 1/2.

#define GGAIN 6*(RMAX*0.025)		//	integration multiplier for gyros 15mv/degree/sec
//#define GGAIN 24*(RMAX*0.025)		//	integration multiplier for gyros 3.75mv/degree/sec
fractional ggain = GGAIN ;

#define KPPitchRoll 256*20
#define KIPitchRoll 2

#define KPYaw 256*10
#define KIYaw 2

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

//	gyro correction vectors:
fractional omegacorrPAcc[] = { 0 , 0 , 0 } ;
fractional omegacorrIAcc[] = { 0 , 0 , 0 } ;

fractional omegacorrPGPS[] = { 0 , 0 , 0 } ;
fractional omegacorrIGPS[] = { 0 , 0 , 0 } ;

//	correction vector integrators ;
union longww AccIntegral[] =  { { 0 } , { 0 } ,  { 0 } } ;
union longww GPSIntegral[] =  { { 0 } , { 0 } ,  { 0 } } ;

//	total of gyro plus corrections:
fractional omegatotal[] = { 0 , 0 , 0 } ;

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

int vref_adj_gyro ;
int vref_adj_accel ;
void read_gyros()
//	fetch the gyro signals and subtract the baseline offset, 
//	and adjust for variations in supply voltage
{
	int gx , gy , gz ;
	vref_adj_gyro = (vref.value>>1) - (vref.offset>>1) ;
	gx = omegagyro[0] = (xrate.value>>1) - (xrate.offset>>1) - vref_adj_gyro ;
	gy = omegagyro[1] = -((yrate.value>>1) - (yrate.offset>>1) - vref_adj_gyro) ; // because yrate sign is flipped
	gz = omegagyro[2] = (zrate.value>>1) - (zrate.offset>>1) - vref_adj_gyro ;
	if ( 	( gx < GYROSAT ) && ( gx > - GYROSAT )
			&&( gy < GYROSAT ) && ( gy > - GYROSAT )
			&&( gz < GYROSAT ) && ( gz > - GYROSAT )  )
	{
		LATFbits.LATF0 = 1 ;
	}
	else
	{
		LATFbits.LATF0 = 0 ;
	}
	return ;
}

void read_accel()
{
	vref_adj_accel = vref_adj_gyro>> 1 ; // actually, its supposed to be 0.625, but this is close enough
	gplane[0] =   ( xaccel.value>>1 ) - ( xaccel.offset>>1 ) - vref_adj_accel ;
	gplane[1] =   ( yaccel.value>>1 ) - ( yaccel.offset>>1 ) - vref_adj_accel ;
	gplane[2] =   ( zaccel.value>>1 ) - ( zaccel.offset>>1 ) - vref_adj_accel ;
	return ;
}

//	multiplies omega times speed, and scales appropriately
int omegaSOG ( int omega , unsigned int speed  )
{
	union longww working ;
	speed = speed>>3 ;
	working.WW = __builtin_mulsu( omega , speed ) ;
	if ( working._.W1 > CENTRIFSAT )
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
	gplane[0]=gplane[0]- omegaSOG( omegagyro[2] , (unsigned int) sog_gps.BB ) ;
	gplane[2]=gplane[2]+ omegaSOG( omegagyro[0] , (unsigned int) sog_gps.BB ) ;
	return ;
}

//	The update algorithm!!
void rupdate(void)
//	This is the key routine. It performs a small rotation
//	on the direction cosine matrix, based on the gyro vector and correction.
//	It uses vector and matrix routines furnished by Microchip.
{
	interruptsOff();
	VectorAdd( 3 , omegatotal , omegagyro , omegacorrPAcc ) ;
	VectorAdd( 3 , omegatotal , omegatotal , omegacorrIAcc ) ;
	VectorAdd( 3 , omegatotal , omegatotal , omegacorrPGPS ) ;
	VectorAdd( 3 , omegatotal , omegatotal , omegacorrIGPS ) ;
	//	scale by the integration factor:
	VectorScale( 3 , theta , omegatotal , ggain ) ;
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
	VectorScale( 3 , omegacorrPAcc , errorRP , KPPitchRoll ) ;

	AccIntegral[0].WW += (__builtin_mulss( errorRP[0] , KIPitchRoll )>>6) ;
	AccIntegral[1].WW += (__builtin_mulss( errorRP[1] , KIPitchRoll )>>6) ;
	AccIntegral[2].WW += (__builtin_mulss( errorRP[2] , KIPitchRoll )>>6) ;

	omegacorrIAcc[0] = AccIntegral[0]._.W1 ;
	omegacorrIAcc[1] = AccIntegral[1]._.W1 ;
	omegacorrIAcc[2] = AccIntegral[2]._.W1 ;
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

		VectorScale( 3 , omegacorrPGPS , errorYawplane , KPYaw ) ;
		flags._.yaw_req = 0 ;
	}
	GPSIntegral[0].WW += (__builtin_mulss( errorYawplane[0] , KIYaw )>>6) ;
	GPSIntegral[1].WW += (__builtin_mulss( errorYawplane[1] , KIYaw )>>6) ;
	GPSIntegral[2].WW += (__builtin_mulss( errorYawplane[2] , KIYaw )>>6) ;

	omegacorrIGPS[0] = GPSIntegral[0]._.W1 ;
	omegacorrIGPS[1] = GPSIntegral[1]._.W1 ;
	omegacorrIGPS[2] = GPSIntegral[2]._.W1 ;	
	interruptsOn();
	return ;
}

void output_matrix(void)
//	This routine makes the direction cosine matrix evident
//	by setting the three servos to the three values in the
//	diagonal of the matrix. Used for debugging purposes
{
	union longww accum ;
//	accum.WW = __builtin_mulss( rmat[6] , 4000 ) ; // indicates roll
//	PDC2 = 3000 + accum._.W1 ;
//	accum.WW = __builtin_mulss( rmat[7] , 4000 ) ; // indicates pitch
//	PDC3 = 3000 + accum._.W1 ;

//	accum.WW = __builtin_mulss( cosine(actual_dir) , 4000 ) ; // indicates output of GPS
//	PDC2 = 3000 + accum._.W1 ;
//	accum.WW = __builtin_mulss( sine(actual_dir) , 4000 ) ; // indicates output of GPS
//	PDC3 = 3000 + accum._.W1 ;

//	accum.WW = __builtin_mulss( rmat[4] , 4000 ) ; // indicates yaw
//	PDC1 = 3000 + accum._.W1 ;
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
	output_matrix() ;	
	return ;
}

