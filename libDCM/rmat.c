
#include "libDCM_internal.h"
#include "mathlibNAV.h"
#include "deadReckoning.h"
#include "gpsParseCommon.h"
#include "../libUDB/magnetometer.h"
#include "../libUDB/heartbeat.h"
#include "../libUDB/ADchannel.h"
#include <math.h>

// These are the routines for maintaining a direction cosine matrix
// that can be used to transform vectors between the earth and plane
// coordinate systems. The 9 direction cosines in the matrix completely
// define the orientation of the plane with respect to the earth.
// The inverse of the matrix is equal to its transpose. This defines
// the so-called orthogonality conditions, which impose 6 constraints on
// the 9 elements of the matrix.

// All numbers are stored in 2.14 format.
// Vector and matrix libraries work in 1.15 format.
// This combination allows values of matrix elements between -2 and +2.
// Multiplication produces results scaled by 1/2.


#define RMAX15 24576 //0b0110000000000000   // 1.5 in 2.14 format

#define CALIBRATION 1.000
#define GGAIN CALIBRATION*SCALEGYRO*6*(RMAX*(1.0/HEARTBEAT_HZ)) // integration multiplier for gyros
fractional ggain[] =  { GGAIN, GGAIN, GGAIN };

uint16_t spin_rate = 0;

// the gains are constant because the gravity vector is normalized
#define KPROLLPITCH ( 2*2048 )
#define KIROLLPITCH ( (uint32_t) 4*2560 / (uint32_t) HEARTBEAT_HZ)


#define KPYAW ( 2*2048 )
#define KIYAW (4*2560/HEARTBEAT_HZ)

#define GYROSAT 15000
// threshold at which gyros may be saturated

// rmat is the matrix of direction cosines relating
// the body and earth coordinate systems.
// The columns of rmat are the axis vectors of the plane,
// as measured in the earth reference frame.
// The rows of rmat are the unit vectors defining the body frame in the earth frame.
// rmat therefore describes the body frame B relative to the Earth frame E
// and in Craig's notation is represented as (B->E)R: LateX format: presupsub{E}{B}R
// To transform a point from body frame to Earth frame, multiply from the left
// with rmat.

// rmat is initialized to the identity matrix in 2.14 fractional format

#ifdef INITIALIZE_VERTICAL  // for VTOL vertical initialization
fractional rmat[] = { RMAX, 0, 0, 0, 0, RMAX, 0, -RMAX, 0 };
static fractional rmatDelayCompensated[] =  { RMAX, 0, 0, 0, 0, RMAX, 0, -RMAX, 0 };

#else // the usual case, horizontal initialization
fractional rmat[] = { RMAX, 0, 0, 0, RMAX, 0, 0, 0, RMAX };
#if (MAG_YAW_DRIFT == 1)
static fractional rmatDelayCompensated[] = { RMAX, 0, 0, 0, RMAX, 0, 0, 0, RMAX };
#endif
#endif

// rup is the rotational update matrix.
// At each time step, the new rmat is equal to the old one, multiplied by rup.
//fractional rup[] = { RMAX, 0, 0, 0, RMAX, 0, 0, 0, RMAX };

// gyro rotation vector:
fractional omegagyro[] = { 0, 0, 0 };
fractional omega[] = { 0, 0, 0 };

// gyro correction vectors:
static fractional omegacorrP[] = { 0, 0, 0 };
fractional omegacorrI[] = { 0, 0, 0 };

// acceleration, as measured in GPS earth coordinate system
fractional accelEarth[] = { 0, 0, 0 };

//union longww accelEarthFiltered[] = { { 0 }, { 0 },  { 0 } };

// correction vector integrators;
static union longww gyroCorrectionIntegral[] =  { { 0 }, { 0 },  { 0 } };

// accumulator for computing adjusted omega:
fractional omegaAccum[] = { 0, 0, 0 };

// gravity, as measured in plane coordinate system
#ifdef INITIALIZE_VERTICAL // VTOL vertical initialization
fractional gplane[] = { 0, -GRAVITY, 0 };
int16_t aero_force[] = { 0 , GRAVITY , 0 };
#else  // horizontal initialization 
fractional gplane[] = { 0, 0, GRAVITY };
int16_t aero_force[] = { 0 , 0 , -GRAVITY };
#endif

// horizontal velocity over ground, as measured by GPS (Vz = 0)
fractional dirOverGndHGPS[] = { 0, RMAX, 0 };

// horizontal direction over ground, as indicated by Rmatrix
fractional dirOverGndHrmat[] = { 0, RMAX, 0 };

// rotation angle equal to omega times integration factor:
//fractional theta[] = { 0, 0, 0 };

// matrix buffer:
//fractional rbuff[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };

// vector buffer
static fractional errorRP[] = { 0, 0, 0 };
static fractional errorYawground[] = { 0, 0, 0 };
static fractional errorYawplane[]  = { 0, 0, 0 };

// measure of error in orthogonality, used for debugging purposes:
static fractional error = 0;

#if (MAG_YAW_DRIFT == 1)
static fractional declinationVector[2];
#endif

#if (DECLINATIONANGLE_VARIABLE == 1)
union intbb dcm_declination_angle;
#endif

void yaw_drift_reset(void)
{
	errorYawground[0] = errorYawground[1] = errorYawground[2] = 0; // turn off yaw drift
}

void dcm_init_rmat(void)
{
#if (MAG_YAW_DRIFT == 1)
#if (DECLINATIONANGLE_VARIABLE == 1)
	dcm_declination_angle.BB = DECLINATIONANGLE;
#endif
	declinationVector[0] = cosine((int8_t) (DECLINATIONANGLE >> 8));
	declinationVector[1] = sine((int8_t) (DECLINATIONANGLE >> 8));
#endif
}

static inline void read_gyros(void)
{
	// fetch the gyro signals and subtract the baseline offset, 
	// and adjust for variations in supply voltage
	
#if (HILSIM == 1)
	HILSIM_set_omegagyro();
//	omegagyro[0] = q_sim.BB;
//	omegagyro[1] = p_sim.BB;
//	omegagyro[2] = r_sim.BB;
#else
	omegagyro[0] = XRATE_VALUE;
	omegagyro[1] = YRATE_VALUE;
	omegagyro[2] = ZRATE_VALUE;
#endif
}

inline void read_accel(void)
{
#if (HILSIM == 1)
	HILSIM_set_gplane();
//	gplane[0] = g_a_x_sim.BB;
//	gplane[1] = g_a_y_sim.BB;
//	gplane[2] = g_a_z_sim.BB;
#else
	gplane[0] = XACCEL_VALUE;
	gplane[1] = YACCEL_VALUE;
	gplane[2] = ZACCEL_VALUE;
	aero_force[0] = - gplane[0] ;
	aero_force[1] = - gplane[1] ;
	aero_force[2] = - gplane[2] ;
#endif

#ifdef CATAPULT_LAUNCH_ENABLE
	if (gplane[1] < -(GRAVITY/2))
	{
		dcm_flags._.launch_detected = 1;
	}
#endif

        // gplane is a vector that represents the gravity vector minus the acceleration vector,
        // as seen in the body frame. Taking the dot products of gplane with the rows of the
        // rotation matrix is the same thing as multiplying the rotation matrix times gplane vector,
        // which is the same thing as transforming gplane into the earth frame,
        // which gives us gravity minus acceleration in the earth frame,
        // regardless of the orientation of the aircraft.
        //
        // We want acceleration minus gravity in the earth frame, so sign flip all three components.
        // Going from UDB coordinates to Earth coordinates (east, north, up) requires us to apply
        // another sign flip to x and z. So, taking the dot products and
        // flipping only the sign of y, gives us acceleration minus gravity in the earth frame.
        //
        // The final step is to add gravity, which is equal to -GRAVITY, since gravity (which is down)
        // is in the opposite direction to the earth up direction. so, the final step is to add -GRAVITY.
        //
        // See the following URL for further details of the frame reference conventions for UDB / MatrixPilot
        // https://code.google.com/p/gentlenav/wiki/UDBCoordinateSystems
	accelEarth[0] = +(VectorDotProduct(3, &rmat[0], gplane) << 1);
	accelEarth[1] = -(VectorDotProduct(3, &rmat[3], gplane) << 1);
	accelEarth[2] = +(-((int16_t)GRAVITY) + (VectorDotProduct(3, &rmat[6], gplane) << 1));

//	accelEarthFiltered[0].WW += ((((int32_t)accelEarth[0])<<16) - accelEarthFiltered[0].WW)>>5;
//	accelEarthFiltered[1].WW += ((((int32_t)accelEarth[1])<<16) - accelEarthFiltered[1].WW)>>5;
//	accelEarthFiltered[2].WW += ((((int32_t)accelEarth[2])<<16) - accelEarthFiltered[2].WW)>>5;
}

void udb_callback_read_sensors(void)
{
	read_gyros(); // record the average values for both DCM and for offset measurements
	read_accel();
}

static void adj_accel(void)
{
	// total (3D) airspeed in cm/sec is used to adjust for acceleration
//	gplane[0] = gplane[0] - omegaSOG(omegaAccum[2], air_speed_3DGPS);
//	gplane[2] = gplane[2] + omegaSOG(omegaAccum[0], air_speed_3DGPS);
//	gplane[1] = gplane[1] + ((uint16_t)(ACCELSCALE)) * forward_acceleration;
}
fractional theta[3];
// The update algorithm!!
static void rupdate(void)
{
	// This is the key routine. It performs a small rotation
	// on the direction cosine matrix, based on the gyro vector and correction.
	// It uses vector and matrix routines furnished by Microchip.
	fractional delta_angle[9];
	fractional delta_angle_square_over_2[9];
	fractional delta_angle_cube_over_6[9];
	fractional rup[9];

	fractional rbuff[9];
		
	VectorAdd(3, omegaAccum, omegagyro, omegacorrI);
	VectorAdd(3, omega, omegaAccum, omegacorrP);
	//	scale by the integration factors:
	VectorMultiply(3, theta, omega, ggain); // Scalegain of 2
	// diagonal elements of the update matrix:
	rup[0] = RMAX;
	rup[4] = RMAX;
	rup[8] = RMAX;
	rup[1] = 0 ;
	rup[2] = 0 ;
	rup[3] = 0 ;
	rup[5] = 0 ;
	rup[6] = 0 ;
	rup[7] = 0 ;

	// construct the delta angle matrix:
	delta_angle[0] = 0 ;
	delta_angle[1] = -theta[2];
	delta_angle[2] =  theta[1];
	delta_angle[3] =  theta[2];
	delta_angle[4] = 0 ;
	delta_angle[5] = -theta[0];
	delta_angle[6] = -theta[1];
	delta_angle[7] =  theta[0];
	delta_angle[8] = 0 ;
	
	// compute 1/2 of square of the delta angle matrix
	// since a matrix multiply divides by 2, we get it for free	
	MatrixMultiply( 3, 3, 3, delta_angle_square_over_2 , delta_angle , delta_angle );
	
	// first step in computing delta angle cube over 6, compute it over 4 ;
	MatrixMultiply( 3, 3, 3, delta_angle_cube_over_6 , delta_angle_square_over_2 , delta_angle );
	
	// multiply by 2/3
	int16_t loop_index ;
	for ( loop_index = 0 ; loop_index <= 8 ; ++ loop_index ) 
	{
		delta_angle_cube_over_6[loop_index] = __builtin_divsd(__builtin_mulsu(delta_angle_cube_over_6[loop_index],2 ),3);
	}
	
	// form the update matrix
	MatrixAdd(3, 3, rup, rup, delta_angle );
	MatrixAdd(3, 3, rup, rup, delta_angle_square_over_2 );
	MatrixAdd(3, 3, rup, rup, delta_angle_cube_over_6 );

	// matrix multiply the rmatrix by the update matrix
	MatrixMultiply(3, 3, 3, rbuff, rmat, rup);
	// multiply by 2 and copy back from rbuff to rmat:
	MatrixAdd(3, 3, rmat, rbuff, rbuff);
}

// The normalization algorithm
static void normalize(void)
{
	//  This is the routine that maintains the orthogonality of the
	//  direction cosine matrix, which is expressed by the identity
	//  relationship that the cosine matrix multiplied by its
	//  transpose should equal the identity matrix.
	//  Small adjustments are made at each time step to assure orthogonality.

	fractional norm;    // actual magnitude
	fractional renorm;  // renormalization factor
	fractional rbuff[9];

	// take the tilt row without any adjustments
	VectorCopy( 3 , &rbuff[6] , &rmat[6] ) ;
	// compute the negative of the dot product between rows 2 and 3
	error =  - 2*VectorDotProduct(3, &rmat[3], &rmat[6]);
	// compute adjustment to row 2 that will make it more perpendicular to row 3
	VectorScale( 3 , &rbuff[3] , &rbuff[6] , error ) ;
	VectorAdd( 3, &rbuff[3] , &rbuff[3] , &rmat[3] ) ;
	// use the cross product of row 2 and 3 to get the first row
	VectorCross(&rbuff[0] , &rbuff[3] , &rbuff[6] ) ;

	// Use a Taylor's expansion for 1/sqrt(X*X) to avoid division in the renormalization

	// rescale row1
	norm = VectorPower(3, &rbuff[0]); // Scalegain of 0.5
	renorm = RMAX15 - norm;
	VectorScale(3, &rbuff[0], &rbuff[0], renorm);
	VectorAdd(3, &rmat[0], &rbuff[0], &rbuff[0]);
	// rescale row2
	norm = VectorPower(3, &rbuff[3]);
	renorm = RMAX15 - norm;
	VectorScale(3, &rbuff[3], &rbuff[3], renorm);
	VectorAdd(3, &rmat[3], &rbuff[3], &rbuff[3]);
	// rescale row3
	norm = VectorPower(3, &rbuff[6]);
	renorm = RMAX15 - norm;
	VectorScale(3, &rbuff[6], &rbuff[6], renorm);
	VectorAdd(3, &rmat[6], &rbuff[6], &rbuff[6]);
}

int16_t accelOn ;
int16_t launched ;
extern int16_t accelEarthVertical ;
extern int32_t velocityEarthVertical ;
int16_t launch_count ;

//#define LAUNCH_ACCELERATION ( 2.0 ) // times gravity
//#define LAUNCH_VELOCITY ( 10.0 ) // miles per hour
//#define LAUNCH_ACCELERATION_BINARY (( int16_t) ( GRAVITY*LAUNCH_ACCELERATION ))
//#define EARTH_GRAVITY ( 9.81 ) // meters per second per second
//#define FRAME_RATE ( 40.0 ) // computations are done 40 times per second
//#define METERSPERSECONDPERMPH ( 4.0/9.0 ) // conversion from MPH to meters/second
//#define LAUNCH_VELOCITY_BINARY ( ( int32_t ) ( LAUNCH_VELOCITY*GRAVITY*FRAME_RATE*METERSPERSECONDPERMPH/ EARTH_GRAVITY ) )
//#define LAUNCH_DETECT_COUNT ( 20 )
//#define GROUND_TEST
static void roll_pitch_drift(void)
{
	//uint16_t gplaneMagnitude  ;
	//uint16_t acceleration ;	
//	gplaneMagnitude = vector3_mag( gplane[0] , gplane[1] , gplane[2]   ) ;
//	acceleration = abs ( gplaneMagnitude - GRAVITY ) ;
	
	if(1) // this is where the logic goes to turn off compensation
	{
		accelOn = 1 ;
		int16_t gplane_nomalized[3] ;
		vector3_normalize( gplane_nomalized , gplane ) ;
		VectorCross(errorRP, gplane_nomalized, &rmat[6]);
		dirOverGndHrmat[0] = (rmat[0])/2 + (rmat[4])/2 ;
		dirOverGndHrmat[1] = (rmat[3])/2 - (rmat[1])/2 ;
		dirOverGndHrmat[2] = 0 ;
		dirOverGndHGPS[0] = RMAX ;
		dirOverGndHGPS[1] = 0 ;
		dirOverGndHGPS[2] = 0 ;
		VectorCross(errorYawground, dirOverGndHrmat , dirOverGndHGPS );
		/*
		if ( rmat[0] > 0 ) // less than 90 degree roll
		{
			VectorCross(errorYawground, dirOverGndHrmat , dirOverGndHGPS );
		}
		else
		{
			errorYawground[0] = 0 ;
			errorYawground[1] = 0 ;
			if ( rmat[3] > 0 )
			{
				errorYawground[2] = -RMAX ;
			}
			else
			{
				errorYawground[2] = RMAX ;
			}
		}
		 */
			// convert to plane frame:
			// *** Note: this accomplishes multiplication rmat transpose times errorYawground!!
		MatrixMultiply(1, 3, 3, errorYawplane, errorYawground, rmat) ;
	}
	else
	{
		accelOn = 0 ;
		errorRP[0] = 0 ;
		errorRP[1] = 0 ;
		errorRP[2] = 0 ;
		errorYawplane[0] = 0 ;
		errorYawplane[1] = 0 ;
		errorYawplane[2] = 0 ;
	}

}

#define MAXIMUM_SPIN_DCM_INTEGRAL 20.0 // degrees per second

static void PI_feedback(void)
{
	fractional errorRPScaled[3];
	int16_t kpyaw;
	int16_t kprollpitch;

	// boost the KPs at high spin rate, to compensate for increased error due to calibration error
	// above 50 degrees/second, scale by rotation rate divided by 50

	{
		kpyaw = KPYAW;
		kprollpitch = KPROLLPITCH;
	}
	
	VectorScale(3, omegacorrP, errorYawplane, kpyaw);   // Scale gain = 2
	VectorScale(3, errorRPScaled, errorRP, kprollpitch);// Scale gain = 2
	VectorAdd(3, omegacorrP, omegacorrP, errorRPScaled);

	// turn off the offset integrator while spinning, it doesn't work in that case,
	// and it only causes trouble.

	{	
		gyroCorrectionIntegral[0].WW += (__builtin_mulsu(errorRP[0], KIROLLPITCH)>>3);
		gyroCorrectionIntegral[1].WW += (__builtin_mulsu(errorRP[1], KIROLLPITCH)>>3);
		gyroCorrectionIntegral[2].WW += (__builtin_mulsu(errorRP[2], KIROLLPITCH)>>3);

		gyroCorrectionIntegral[0].WW += (__builtin_mulsu(errorYawplane[0], KIYAW)>>3);
		gyroCorrectionIntegral[1].WW += (__builtin_mulsu(errorYawplane[1], KIYAW)>>3);
		gyroCorrectionIntegral[2].WW += (__builtin_mulsu(errorYawplane[2], KIYAW)>>3);
	}

	omegacorrI[0] = gyroCorrectionIntegral[0]._.W1>>3;
	omegacorrI[1] = gyroCorrectionIntegral[1]._.W1>>3;
	omegacorrI[2] = gyroCorrectionIntegral[2]._.W1>>3;
}


/*
void output_matrix(void)
{
	// This routine makes the direction cosine matrix evident by setting 
	// the three servos to the three values in the matrix.
	union longww accum;
	accum.WW = __builtin_mulss(rmat[6], 4000);
//	PDC1 = 3000 + accum._.W1;
//	accum.WW = __builtin_mulss(rmat[7], 4000);
	accum.WW = __builtin_mulss(rmat[3], 4000);
	PDC2 = 3000 + accum._.W1;
	accum.WW = __builtin_mulss(rmat[4], 4000);
	PDC3 = 3000 + accum._.W1;
}
 */

/*
void output_IMUvelocity(void)
{
	PDC1 = pulsesat(IMUvelocityx._.W1 + 3000);
	PDC2 = pulsesat(IMUvelocityy._.W1 + 3000);
	PDC3 = pulsesat(IMUvelocityz._.W1 + 3000);

//	PDC1 = pulsesat(accelEarth[0] + 3000);
//	PDC2 = pulsesat(accelEarth[1] + 3000);
//	PDC3 = pulsesat(accelEarth[2] + 3000);
}
 */

void dcm_run_imu_step(void)
{
	// update the matrix, renormalize it, adjust for roll and
	// pitch drift, and send it to the servos.
//	dead_reckon();              // in libDCM:deadReconing.c
	adj_accel();                // local
#if ( GYRO_RANGE == 500 )
	rupdate();                  // local
#elif ( GYRO_RANGE == 1000 )
	rupdate();                  // local
	rupdate();                  // local
#else
#error set GYRO_RANGE to 500 or 1000 in options.h
#endif // GYRO_RANGE
	normalize();                // local
	roll_pitch_drift();         // local
#if (MAG_YAW_DRIFT == 1)
//	// TODO: validate: disabling mag_drift when airspeed greater than 5 m/sec
//	if ((magMessage == 7) && (air_speed_3DIMU < 500))
	if (magMessage == 7)
	{
//		mag_drift();            // local
	}
	else
	{
//		yaw_drift();            // local
	}
#else
//	yaw_drift();                // local
#endif
	PI_feedback();              // local
//	calibrate_gyros();          // local
}
float roll_angle , pitch_angle , yaw_angle ;
float rmat_f[9];
#define DEG_PER_RAD 57.296
void compute_euler(void)
{
	rmat_f[0]=(float)rmat[0] ;
	rmat_f[1]=(float)rmat[1] ;
	rmat_f[2]=(float)rmat[2] ;
	rmat_f[3]=(float)rmat[3] ;
	rmat_f[4]=(float)rmat[4] ;
	rmat_f[5]=(float)rmat[5] ;
	rmat_f[6]=(float)rmat[6] ;
	rmat_f[7]=(float)rmat[7] ;
	rmat_f[8]=(float)rmat[8] ;

	pitch_angle = DEG_PER_RAD*atan2f(-rmat_f[6],sqrtf(rmat_f[7]*rmat_f[7]+rmat_f[8]*rmat_f[8]));
	roll_angle = DEG_PER_RAD*atan2f(rmat_f[7],rmat_f[8]);
	yaw_angle = DEG_PER_RAD*atan2f(rmat_f[3],rmat_f[0]);
}
