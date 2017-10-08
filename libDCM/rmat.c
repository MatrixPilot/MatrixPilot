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


#include "libDCM.h"
#include "mathlibNAV.h"
#include "deadReckoning.h"
#include "gpsParseCommon.h"
#include "../libUDB/heartbeat.h"
#include "../libUDB/ADchannel.h"
#include "../libUDB/magnetometer.h"
#include "options_magnetometer.h"
#include "mag_drift.h"
#include "rmat.h"

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

#define GGAIN SCALEGYRO*6*(RMAX*(1.0/HEARTBEAT_HZ)) // integration multiplier for gyros
static fractional ggain[] =  { GGAIN, GGAIN, GGAIN };

static uint16_t spin_rate = 0;
static fractional spin_axis[] = { 0, 0, RMAX };

#if (BOARD_TYPE == AUAV3_BOARD || BOARD_TYPE == UDB5_BOARD || BOARD_TYPE == PX4_BOARD)
// modified gains for MPU6000
#define KPROLLPITCH (ACCEL_RANGE * 1280/3)
#define KIROLLPITCH (ACCEL_RANGE * 3400 / HEARTBEAT_HZ)

#elif (BOARD_TYPE == UDB4_BOARD)
// Paul's gains for 6G accelerometers
#define KPROLLPITCH (256*5)
#define KIROLLPITCH (10240/HEARTBEAT_HZ) // 256

#else
#error Unsupported BOARD_TYPE
#endif // BOARD_TYPE

#define KPYAW 256*4
//#define KIYAW 32
#define KIYAW (1280/HEARTBEAT_HZ)

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
#else // the usual case, horizontal initialization
fractional rmat[] = { RMAX, 0, 0, 0, RMAX, 0, 0, 0, RMAX };
#endif

// rup is the rotational update matrix.
// At each time step, the new rmat is equal to the old one, multiplied by rup.
//fractional rup[] = { RMAX, 0, 0, 0, RMAX, 0, 0, 0, RMAX };

// gyro rotation vector:
fractional omegagyro[] = { 0, 0, 0 };
static fractional omega[] = { 0, 0, 0 };

// gyro correction vectors:
static fractional omegacorrP[] = { 0, 0, 0 };
static fractional omegacorrI[] = { 0, 0, 0 };

// acceleration, as measured in GPS earth coordinate system
fractional accelEarth[] = { 0, 0, 0 };

//union longww accelEarthFiltered[] = { { 0 }, { 0 },  { 0 } };

// correction vector integrators;
static union longww gyroCorrectionIntegral[] =  { { 0 }, { 0 },  { 0 } };

// accumulator for computing adjusted omega:
fractional omegaAccum[] = { 0, 0, 0 };

// gplane[] is a vector representing (gravity - acceleration) in the plane's coordinate system. 
// gravity_vector_plane[], is gravity as measured in the plane's coordinate system
#ifdef INITIALIZE_VERTICAL // VTOL vertical initialization
static fractional gplane[] = { 0, -GRAVITY, 0 };
static fractional gravit_vector_plane[] = { 0, -GRAVITY, 0 };
int16_t aero_force[] = { 0 , GRAVITY , 0 };
#else  // horizontal initialization
static fractional gplane[] = { 0, 0, GRAVITY };
static fractional gravity_vector_plane[] = { 0, 0, GRAVITY };
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

void yaw_drift_reset(void)
{
	errorYawground[0] = errorYawground[1] = errorYawground[2] = 0; // turn off yaw drift
}

void dcm_init_rmat(void)
{
#if (MAG_YAW_DRIFT == 1)
	mag_drift_init();
//#if (DECLINATIONANGLE_VARIABLE == 1)
//	dcm_declination_angle.BB = DECLINATIONANGLE;
//#endif
//	declinationVector[0] = cosine((int8_t) (DECLINATIONANGLE >> 8));
//	declinationVector[1] = sine((int8_t) (DECLINATIONANGLE >> 8));
#endif
}

static inline void read_gyros(void)
{
	// fetch the gyro signals and subtract the baseline offset,
	// and adjust for variations in supply voltage
	unsigned spin_rate_over_2;

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

	spin_rate = vector3_mag(omegagyro[0], omegagyro[1], omegagyro[2]);
	spin_rate_over_2 = spin_rate >> 1;

	if (spin_rate_over_2 > 0)
	{
		spin_axis[0] = __builtin_divsd(((int32_t)omegagyro[0]) << 13, spin_rate_over_2);
		spin_axis[1] = __builtin_divsd(((int32_t)omegagyro[1]) << 13, spin_rate_over_2);
		spin_axis[2] = __builtin_divsd(((int32_t)omegagyro[2]) << 13, spin_rate_over_2);
	}
}

static inline void read_accel(void)
{
#if (HILSIM == 1)
	HILSIM_set_gplane(gplane);
#else
	gplane[0] = XACCEL_VALUE;
	gplane[1] = YACCEL_VALUE;
	gplane[2] = ZACCEL_VALUE;
#endif
	aero_force[0] = - gplane[0];
	aero_force[1] = - gplane[1];
	aero_force[2] = - gplane[2];

#ifdef CATAPULT_LAUNCH_ENABLE
	if (gplane[1] < -(GRAVITY/2))
	{
		dcm_flags._.launch_detected = 1;
	}
#endif

	// transform gplane from body frame to earth frame
	// x component in earth frame is earth x unit vector (rmat[0,1,2]) dotted with gplane

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

static int16_t omegaSOG(int16_t omega, int16_t speed)
{
	// multiplies omega times speed, and scales appropriately
	// omega in radians per second, speed in cm per second
	union longww working;
	speed = speed >> 3;
	working.WW = __builtin_mulss(omega, speed);
	if (((int16_t)working._.W1) > ((int16_t)CENTRIFSAT))
	{
		return RMAX;
	}
	else if (((int16_t)working._.W1) < ((int16_t)-CENTRIFSAT))
	{
		return - RMAX;
	}
	else
	{
		working.WW = working.WW>>5;
		working.WW = __builtin_mulsu(working._.W0, CENTRISCALE);
		working.WW = working.WW<<5;
		return working._.W1;
	}
}

static void adj_accel(int16_t angleOfAttack)
{
	// Performs centrifugal compensation without a GPS.
	// Based on the fact that the magnitude of the
	// compensated gplane vector should be GRAVITY*GRAVITY.
	// This produces another equation from which the
	// product of airspeed time rotation rate can be reasonably estimated.
	int16_t omega_times_velocity ; // it should be positive, but noise
	                               // in the computations could produce neg
	uint16_t radical;
	union longww accum;
	int16_t accelY;
	int16_t vertical_cross_rotation_axis;
	int16_t force_cross_rotation_axis;
	int16_t rotation_axis[2];

	// Compute the X-Z rotation axis
	// by normalizing the X-Z gyro vector
	rotation_axis[0] = omegagyro[0];
	rotation_axis[1] = omegagyro[2];
	vector2_normalize(rotation_axis, rotation_axis);

	// compute force cross rotation axis:
	accum.WW = (__builtin_mulss(gplane[0], rotation_axis[1]) - __builtin_mulss( gplane[2] , rotation_axis[0] ) ) << 2;
	force_cross_rotation_axis = accum._.W1;

	// compute vertical cross rotation axis:
	accum.WW = (__builtin_mulss(rmat[6], rotation_axis[1]) - __builtin_mulss(rmat[8], rotation_axis[0] ) ) << 2;
	vertical_cross_rotation_axis = accum._.W1;

	// compute the square root of the sum of the square of the
	// force cross rotation, minus the square of the magnitude of the accelerometer vector,
	// plus the square of GRAVITY

	// Start by using rmat for accelY instead of the measured value.
	// It is less sensitive to forward acceleration, which cannot be compensated without GPS.
	accum.WW = (__builtin_mulsu( rmat[7], GRAVITY ) ) << 2;
	accelY = accum._.W1;

	// form the sum
	accum.WW = __builtin_mulss(force_cross_rotation_axis, force_cross_rotation_axis)
	         + __builtin_muluu(GRAVITY, GRAVITY)
	         - __builtin_mulss(gplane[0], gplane[0])
	         - __builtin_mulss(gplane[2], gplane[2])
	         - __builtin_mulss(accelY, accelY);
	if (accum.WW < 0)
	{
		accum.WW = 0;
	}
	radical = sqrt_long((uint32_t)accum.WW);

	// Now we are using the solution to quadratic equation in the theory,
	// and there is some logic for selecting the positive or negative square root
	if (force_cross_rotation_axis < 0)
	{
		omega_times_velocity = force_cross_rotation_axis + radical;
	}
	else
	{
		if (vertical_cross_rotation_axis < 0)
		{
			omega_times_velocity = force_cross_rotation_axis + radical;
		}
		else
		{
			omega_times_velocity = force_cross_rotation_axis - radical;
		}
	}
	if (omega_times_velocity < 0)
	{
		omega_times_velocity = 0;
	}
	// now compute omega vector cross velocity vector and adjust
	accum.WW = (__builtin_mulss(omega_times_velocity , rotation_axis[1] ) ) << 2;
	gravity_vector_plane[0] = gplane[0] - accum._.W1;
	gravity_vector_plane[1] = gplane[1];
	accum.WW = (__builtin_mulss(omega_times_velocity , rotation_axis[0] ) ) << 2;
	gravity_vector_plane[2] = gplane[2] + accum._.W1;
	
	// account for angle of attack and forward acceleration
	int16_t air_speed_z;
	// total (3D) airspeed in cm/sec is used to adjust for acceleration
	// compute Z component of airspeed due to angle of attack
	accum.WW = __builtin_mulsu(angleOfAttack, air_speed_3DGPS) << 2;
	air_speed_z = accum._.W1;
	// compute centrifugal and forward acceleration compensation
	gravity_vector_plane[0] = gravity_vector_plane[0] + omegaSOG(omegaAccum[1], air_speed_z);
	gravity_vector_plane[1] = gravity_vector_plane[1] - omegaSOG(omegaAccum[0], air_speed_z) + ((uint16_t)(ACCELSCALE)) * forward_acceleration;

}

// The update algorithm!!
static void rupdate(void)
{
	// This is the key routine. It performs a small rotation
	// on the direction cosine matrix, based on the gyro vector and correction.
	// It uses vector and matrix routines furnished by Microchip.
	fractional rup[9];
	fractional theta[3];
	fractional rbuff[9];
	uint32_t thetaSquare;
	unsigned nonlinearAdjust;

	VectorAdd(3, omegaAccum, omegagyro, omegacorrI);
	VectorAdd(3, omega, omegaAccum, omegacorrP);
	//	scale by the integration factors:
	VectorMultiply(3, theta, omega, ggain); // Scalegain of 2
	// diagonal elements of the update matrix:
	rup[0] = rup[4] = rup[8]= RMAX;

	// compute the square of rotation
	thetaSquare = __builtin_mulss (theta[0], theta[0]) +
	              __builtin_mulss (theta[1], theta[1]) +
	              __builtin_mulss (theta[2], theta[2]);

	// adjust gain by rotation_squared divided by 3
	nonlinearAdjust = RMAX + ((uint16_t) (thetaSquare >>14))/3;

	theta[0] = __builtin_mulsu (theta[0], nonlinearAdjust)>>14;
	theta[1] = __builtin_mulsu (theta[1], nonlinearAdjust)>>14;
	theta[2] = __builtin_mulsu (theta[2], nonlinearAdjust)>>14;

	// construct the off-diagonal elements of the update matrix:
	rup[1] = -theta[2];
	rup[2] =  theta[1];
	rup[3] =  theta[2];
	rup[5] = -theta[0];
	rup[6] = -theta[1];
	rup[7] =  theta[0];

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
	VectorCopy( 9 , rbuff , rmat ); // copy direction cosine matrix into buffer
	
	// Leave the bottom (tilt) row alone, it is usually the most accurate.
	// Compute the first row as the cross product of second row with third row.
	VectorCross(&rbuff[0], &rbuff[3] , &rbuff[6]);
	// First row is now perpendicular to the second and third row.
	// Compute the second row as the cross product of the third row with the first row.
	VectorCross(&rbuff[3], &rbuff[6] , &rbuff[0]);
	// All three rows are now mutually perpendicular.

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

static void roll_pitch_drift(void)
{
	VectorCross(errorRP, gravity_vector_plane, &rmat[6]);
//#ifdef CATAPULT_LAUNCH_ENABLE
#define MAXIMUM_PITCH_ERROR ((fractional)(GRAVITY*0.25))
	// the following is done to limit the pitch error during a catapult launch
	// it has no effect during normal conditions, because acceleration
	// compensated gravity_vector_plane is approximately aligned with rmat[6] vector
	if (errorRP[0] >  MAXIMUM_PITCH_ERROR) errorRP[0] =  MAXIMUM_PITCH_ERROR;
	if (errorRP[0] < -MAXIMUM_PITCH_ERROR) errorRP[0] = -MAXIMUM_PITCH_ERROR;
//#endif // CATAPULT_LAUNCH_ENABLE
}

static void yaw_drift(void)
{
	// although yaw correction is done in horizontal plane,
	// this is done in 3 dimensions, just in case we change our minds later
	// form the horizontal direction over ground based on rmat
	if (dcm_flags._.yaw_req)
	{
		if (ground_velocity_magnitudeXY > GPS_SPEED_MIN)
		{
			// vector cross product to get the rotation error in ground frame
			VectorCross(errorYawground, dirOverGndHrmat, dirOverGndHGPS);
			// convert to plane frame:
			// *** Note: this accomplishes multiplication rmat transpose times errorYawground!!
			MatrixMultiply(1, 3, 3, errorYawplane, errorYawground, rmat);
		}
		else
		{
			errorYawplane[0] = errorYawplane[1] = errorYawplane[2] = 0;
		}
		dcm_flags._.yaw_req = 0;
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

	if (spin_rate < ((uint16_t)(50.0 * DEGPERSEC)))
	{
		kpyaw = KPYAW;
		kprollpitch = KPROLLPITCH;
	}
	else if (spin_rate < ((uint16_t)(500.0 * DEGPERSEC)))
	{
		kpyaw = ((uint16_t)((KPYAW * 8.0) / (50.0 * DEGPERSEC))) * (spin_rate >> 3);
		kprollpitch = ((uint16_t)((KPROLLPITCH * 8.0) / (50.0 * DEGPERSEC))) * (spin_rate >> 3);
	}
	else
	{
		kpyaw = (int16_t)(10.0 * KPYAW);
		kprollpitch = (int16_t)(10.0 * KPROLLPITCH);
	}
	VectorScale(3, omegacorrP, errorYawplane, kpyaw);   // Scale gain = 2
	VectorScale(3, errorRPScaled, errorRP, kprollpitch);// Scale gain = 2
	VectorAdd(3, omegacorrP, omegacorrP, errorRPScaled);

	// turn off the offset integrator while spinning, it doesn't work in that case,
	// and it only causes trouble.

	if (spin_rate < ((uint16_t) (MAXIMUM_SPIN_DCM_INTEGRAL * DEGPERSEC)))
	{
		gyroCorrectionIntegral[0].WW += (__builtin_mulss(errorRP[0], KIROLLPITCH)>>3);
		gyroCorrectionIntegral[1].WW += (__builtin_mulss(errorRP[1], KIROLLPITCH)>>3);
		gyroCorrectionIntegral[2].WW += (__builtin_mulss(errorRP[2], KIROLLPITCH)>>3);

		gyroCorrectionIntegral[0].WW += (__builtin_mulss(errorYawplane[0], KIYAW)>>3);
		gyroCorrectionIntegral[1].WW += (__builtin_mulss(errorYawplane[1], KIYAW)>>3);
		gyroCorrectionIntegral[2].WW += (__builtin_mulss(errorYawplane[2], KIYAW)>>3);
	}

	omegacorrI[0] = gyroCorrectionIntegral[0]._.W1>>3;
	omegacorrI[1] = gyroCorrectionIntegral[1]._.W1>>3;
	omegacorrI[2] = gyroCorrectionIntegral[2]._.W1>>3;
}

static uint16_t adjust_gyro_gain(uint16_t old_gain, int16_t gain_change)
{
	uint16_t gain;
	gain = old_gain + gain_change;
	if (gain > (uint16_t)(1.1 * GGAIN))
	{
		gain = (uint16_t)(1.1 * GGAIN);
	}
	if (gain < (uint16_t)(0.9 * GGAIN))
	{
		gain = (uint16_t)(0.9 * GGAIN);
	}
	return gain;
}

#define GYRO_CALIB_TAU 10.0
#define MINIMUM_SPIN_RATE_GYRO_CALIB 50.0 // degrees/second

static void calibrate_gyros(void)
{
	fractional omegacorrPweighted[3];
	int32_t calib_accum;
	int16_t gain_change;
	uint16_t spin_rate_over2;

	if (spin_rate > (uint16_t) (MINIMUM_SPIN_RATE_GYRO_CALIB * DEGPERSEC))
	{
		spin_rate_over2 = spin_rate>>1;
		VectorMultiply(3, omegacorrPweighted, spin_axis, omegacorrP); // includes 1/2

		calib_accum = __builtin_mulsu(omegacorrPweighted[0], (uint16_t)(0.025*GGAIN/GYRO_CALIB_TAU));
		gain_change = __builtin_divsd(calib_accum, spin_rate_over2);
		ggain[0] = adjust_gyro_gain(ggain[0], gain_change);

		calib_accum = __builtin_mulsu(omegacorrPweighted[1], (uint16_t)(0.025*GGAIN/GYRO_CALIB_TAU));
		gain_change = __builtin_divsd(calib_accum, spin_rate_over2);
		ggain[1] = adjust_gyro_gain(ggain[1], gain_change);

		calib_accum = __builtin_mulsu(omegacorrPweighted[2], (uint16_t)(0.025*GGAIN/GYRO_CALIB_TAU));
		gain_change = __builtin_divsd(calib_accum, spin_rate_over2);
		ggain[2] = adjust_gyro_gain(ggain[2], gain_change);
	}
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

extern void mag_drift(fractional errorYawplane[]);

void dcm_run_imu_step(int16_t angleOfAttack)
{
	// update the matrix, renormalize it, adjust for roll and
	// pitch drift, and send it to the servos.
	dead_reckon();              // in libDCM:deadReconing.c
	adj_accel(angleOfAttack);   // local
	rupdate();                  // local
	normalize();                // local
	roll_pitch_drift();         // local
#if (MAG_YAW_DRIFT == 1)
//	// TODO: validate: disabling mag_drift when airspeed greater than 5 m/sec
//	if ((magMessage == 7) && (air_speed_3DIMU < 500))
	if (magMessage == 7)
	{
//		mag_drift();            // local
		mag_drift(errorYawplane); // in libDCM:mag_drift.c
	}
	else
	{
		yaw_drift();            // local
	}
#else
	yaw_drift();                // local
#endif
	PI_feedback();              // local
	calibrate_gyros();          // local
}
