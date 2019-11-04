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
#include "../libUDB/heartbeat.h"
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

union intbb dcm_declination_angle;

#if (MAG_YAW_DRIFT == 1)

fractional magFieldEarth[3];
//extern fractional udb_magFieldBody[3];
//extern fractional udb_magOffset[3];
fractional rmatPrevious[9];
fractional magFieldEarthNormalizedPrevious[3];
fractional magAlignment[4] = { 0, 0, 0, RMAX };
fractional magFieldBodyMagnitudePrevious;
fractional magFieldBodyPrevious[3];

#ifdef INITIALIZE_VERTICAL  // for VTOL vertical initialization
static fractional rmatDelayCompensated[] =  { RMAX, 0, 0, 0, 0, RMAX, 0, -RMAX, 0 };
#else // the usual case, horizontal initialization
static fractional rmatDelayCompensated[] = { RMAX, 0, 0, 0, RMAX, 0, 0, 0, RMAX };
#endif

static fractional declinationVector[2];

void mag_drift_init(void) // TODO: can this be called during align_rmat_to_mag below?
{
#if (MAG_YAW_DRIFT == 1)
#if (DECLINATIONANGLE_VARIABLE == 1)
	dcm_declination_angle.BB = DECLINATIONANGLE;
#endif
	declinationVector[0] = cosine((int8_t) (DECLINATIONANGLE >> 8));
	declinationVector[1] = sine((int8_t) (DECLINATIONANGLE >> 8));
#endif
}

static void align_rmat(int16_t costheta, int16_t sintheta)
{
	rmat[0] = costheta;
#ifdef INITIALIZE_VERTICAL // vertical initialization for VTOL
	rmat[5] = costheta;
	rmat[2] = sintheta;
#else
	rmat[4] = costheta;
	rmat[1] = sintheta;
#endif
	rmat[3] = -sintheta;
}

#ifdef INITIALIZE_VERTICAL // vertical initialization for VTOL
static void align_rmat_to_mag(void)
{
	uint8_t theta;
	struct relative2D initialBodyField;
//	int16_t costheta;
//	int16_t sintheta;
	initialBodyField.x = udb_magFieldBody[0];
	initialBodyField.y = udb_magFieldBody[2];
#if(DECLINATIONANGLE_VARIABLE == 1)
	theta = rect_to_polar(&initialBodyField) -64 - (dcm_declination_angle._.B1);
#else
	theta = rect_to_polar(&initialBodyField) -64 - (DECLINATIONANGLE >> 8);
#endif
//	costheta = cosine(theta);
//	sintheta = sine(theta);
//	rmat[0] = rmat[5] = costheta;
//	rmat[2] = sintheta;
//	rmat[3] = - sintheta;
	align_rmat(cosine(theta), sine(theta));
}

#else // horizontal initialization for usual cases
static void align_rmat_to_mag(void)
{
	uint8_t theta;
	struct relative2D initialBodyField;
//	int16_t costheta;
//	int16_t sintheta;
	initialBodyField.x = udb_magFieldBody[0];
	initialBodyField.y = udb_magFieldBody[1];
#if(DECLINATIONANGLE_VARIABLE == 1)
	theta = rect_to_polar(&initialBodyField) -64 - (dcm_declination_angle._.B1);
#else
	theta = rect_to_polar(&initialBodyField) -64 - (DECLINATIONANGLE >> 8);
#endif
//	costheta = cosine(theta);
//	sintheta = sine(theta);
//	rmat[0] = rmat[4] = costheta;
//	rmat[1] = sintheta;
//	rmat[3] = - sintheta;
	align_rmat(cosine(theta), sine(theta));
}
#endif // INITIALIZE_VERTICAL

static void quaternion_adjust(fractional quaternion[], fractional direction[])
{
	// performs an adjustment to a quaternion representation of re-alignement.
	// the cross product is left out, theory and test both show it should not be used.
	fractional delta_cos;
	fractional vector_buffer[3];
	fractional increment[3];
	uint32_t magnitudesqr;
	unsigned magnitude;

	increment[0] = direction[0]>>3;
	increment[1] = direction[1]>>3;
	increment[2] = direction[2]>>3;
	// change is cosine is 1/2 of the dot product of first 3 elements of quaternion
	// with the increment. The 1/2 is built into the dot product.
	delta_cos = - VectorDotProduct(3, quaternion, increment);
	// the change in the first 3 elements is 1/2 of the 4 element times the increment.
	// There is a 1/2 built into the VectorScale
	VectorScale(3, vector_buffer, increment, quaternion[3]);
	// Update the first three components
	VectorAdd(3, quaternion, quaternion, vector_buffer);
	// Update the 4th component
	quaternion[3] += delta_cos;
	// Renormalize
	magnitudesqr = __builtin_mulss(quaternion[0], quaternion[0])
	             + __builtin_mulss(quaternion[1], quaternion[1])
	             + __builtin_mulss(quaternion[2], quaternion[2])
	             + __builtin_mulss(quaternion[3], quaternion[3]);
	magnitude = sqrt_long(magnitudesqr);
	if (0 != magnitude)
	{
		quaternion[0] = __builtin_divsd(__builtin_mulsu(quaternion[0], RMAX), magnitude);
		quaternion[1] = __builtin_divsd(__builtin_mulsu(quaternion[1], RMAX), magnitude);
		quaternion[2] = __builtin_divsd(__builtin_mulsu(quaternion[2], RMAX), magnitude);
		quaternion[3] = __builtin_divsd(__builtin_mulsu(quaternion[3], RMAX), magnitude);
	}
}

static void RotVector2RotMat(fractional rotation_matrix[], fractional rotation_vector[])
{
	// rotation vector represents a rotation in vector form
	// around an axis equal to the normalized value of the vector.
	// It is assumed that rotation_vector already includes a factor of sin(alpha/2)
	// maximum rotation is plus minus 180 degrees.
	fractional cos_alpha;
	fractional cos_half_alpha;
	fractional cos_half_alpha_rotation_vector[3];
	union longww sin_half_alpha_sqr = { 0 };
	int16_t matrix_index;

	cos_half_alpha = rotation_vector[3];

	// compute the square of sine of half alpha
	for (matrix_index = 0; matrix_index < 3; matrix_index++)
	{
		sin_half_alpha_sqr.WW += __builtin_mulss(rotation_vector[matrix_index], rotation_vector[matrix_index]);
	}
	if (sin_half_alpha_sqr.WW > ((int32_t)RMAX*RMAX - 1))
	{
		sin_half_alpha_sqr.WW = (int32_t)RMAX*RMAX - 1;
	}

	// compute cos_alpha
	sin_half_alpha_sqr.WW *= 8;
	cos_alpha = RMAX - sin_half_alpha_sqr._.W1;

	// scale rotation_vector by 2*cos_half_alpha
	VectorScale (3, cos_half_alpha_rotation_vector,  rotation_vector, cos_half_alpha);
	for (matrix_index = 0; matrix_index < 3; matrix_index++)
	{
		cos_half_alpha_rotation_vector[matrix_index] *= 4;
	}

	// compute 2 times rotation_vector times its transpose
	MatrixMultiply(3, 1, 3, rotation_matrix, rotation_vector, rotation_vector);
	for (matrix_index = 0; matrix_index < 9; matrix_index++)
	{
		rotation_matrix[matrix_index] *= 4;
	}

	rotation_matrix[0] += cos_alpha;
	rotation_matrix[4] += cos_alpha;
	rotation_matrix[8] += cos_alpha;

	rotation_matrix[1] -= cos_half_alpha_rotation_vector[2];
	rotation_matrix[2] += cos_half_alpha_rotation_vector[1];
	rotation_matrix[3] += cos_half_alpha_rotation_vector[2];
	rotation_matrix[5] -= cos_half_alpha_rotation_vector[0];
	rotation_matrix[6] -= cos_half_alpha_rotation_vector[1];
	rotation_matrix[7] += cos_half_alpha_rotation_vector[0];
}

#define MAG_LATENCY 0.085 // seconds
#define MAG_LATENCY_COUNT ((int16_t)(HEARTBEAT_HZ * MAG_LATENCY))

// Since mag_drift is called every heartbeat the first assignment to rmatDelayCompensated
// will occur at udb_heartbeat_counter = (1/I2C_SENSOR_RATE - MAG_LATENCY) seconds.
// Since rxMagnetometer is called  at I2C_SENSOR_RATE, this initial
// delay offsets the I2C_SENSOR_RATE Hz updates of rmatDelayCompensated by MAG_LATENCY seconds.
static int16_t mag_latency_counter = (HEARTBEAT_HZ /I2C_SENSOR_RATE) - MAG_LATENCY_COUNT;//200/6-200*0.085=16

//static void mag_drift(void)
//void mag_drift(fractional rmatDelayCompensated[], fractional errorYawplane[])
void mag_drift(fractional errorYawplane[])
{
	int16_t mag_error;
	fractional magFieldEarthNormalized[3];
	fractional magFieldEarthHorzNorm[2];
	fractional magAlignmentError[3];
	fractional rmat2Transpose[9];
	fractional R2TR1RotationVector[3];
	fractional R2TAlignmentErrorR1[3];
	fractional rmatBufferA[9];
	fractional rmatBufferB[9];
	fractional magAlignmentAdjustment[3];
	fractional vectorBuffer[3];
	fractional magFieldBodyMagnitude;
	fractional offsetEstimate[3];

	// the following compensates for magnetometer drift by adjusting the timing
	// of when rmat is read
	mag_latency_counter --;
	if (mag_latency_counter == 0)
	{//rmatDelayCompensated is uodated with the rmat values 16*0.005=80 ms before
		VectorCopy(9, rmatDelayCompensated, rmat);
		mag_latency_counter = (HEARTBEAT_HZ / I2C_SENSOR_RATE);   // not really needed, but its good insurance
		// mag_latency_counter is assigned in the next block
	}

	if (dcm_flags._.mag_drift_req)
	{
		// Compute magnetic offsets
		magFieldBodyMagnitude =	vector3_mag(udb_magFieldBody[0], udb_magFieldBody[1], udb_magFieldBody[2]);
		VectorSubtract(3, vectorBuffer, udb_magFieldBody, magFieldBodyPrevious);
		vector3_normalize(vectorBuffer, vectorBuffer);
		VectorScale(3, offsetEstimate, vectorBuffer, magFieldBodyMagnitude - magFieldBodyMagnitudePrevious);
		VectorCopy(3, magFieldBodyPrevious, udb_magFieldBody);
		magFieldBodyMagnitudePrevious = magFieldBodyMagnitude;

		// Compute and apply the magnetometer alignment adjustment in the body frame
		RotVector2RotMat(rmatBufferA, magAlignment);
		vectorBuffer[0] = VectorDotProduct(3, &rmatBufferA[0], udb_magFieldBody) << 1;
		vectorBuffer[1] = VectorDotProduct(3, &rmatBufferA[3], udb_magFieldBody) << 1;
		vectorBuffer[2] = VectorDotProduct(3, &rmatBufferA[6], udb_magFieldBody) << 1;
		VectorCopy(3, udb_magFieldBody, vectorBuffer);

		if (dcm_flags._.first_mag_reading == 1)
		{
			mag_drift_init();
			align_rmat_to_mag();
			VectorCopy(9, rmatDelayCompensated, rmat);
		}

		mag_latency_counter = (HEARTBEAT_HZ / I2C_SENSOR_RATE) - MAG_LATENCY_COUNT; // setup for the next reading

		// Compute the mag field in the earth frame

		magFieldEarth[0] = VectorDotProduct(3, &rmatDelayCompensated[0], udb_magFieldBody)<<1;
		magFieldEarth[1] = VectorDotProduct(3, &rmatDelayCompensated[3], udb_magFieldBody)<<1;
		magFieldEarth[2] = VectorDotProduct(3, &rmatDelayCompensated[6], udb_magFieldBody)<<1;

		// Normalize the magnetic vector to RMAT
		vector3_normalize(magFieldEarthNormalized, magFieldEarth);
		vector2_normalize(magFieldEarthHorzNorm, magFieldEarth);

		// Use the magnetometer to detect yaw drift
#if (DECLINATIONANGLE_VARIABLE == 1)
		declinationVector[0] = cosine(dcm_declination_angle._.B1);
		declinationVector[1] = sine(dcm_declination_angle._.B1);
#endif
		mag_error = VectorDotProduct(2, magFieldEarthHorzNorm, declinationVector);
		VectorScale(3, errorYawplane, &rmat[6], mag_error); // Scalegain = 1/2

		// Do the computations needed to compensate for magnetometer misalignment

		// Determine the apparent shift in the earth's magnetic field:
		VectorCross(magAlignmentError, magFieldEarthNormalizedPrevious, magFieldEarthNormalized);

		// Compute R2 transpose
		MatrixTranspose(3, 3, rmat2Transpose, rmatDelayCompensated);

		// Compute 1/2 of R2tranpose times R1
		MatrixMultiply(3, 3, 3, rmatBufferA, rmat2Transpose, rmatPrevious);

		// Convert to a rotation vector, take advantage of 1/2 from the previous step
		R2TR1RotationVector[0] = rmatBufferA[7] - rmatBufferA[5];
		R2TR1RotationVector[1] = rmatBufferA[2] - rmatBufferA[6];
		R2TR1RotationVector[2] = rmatBufferA[3] - rmatBufferA[1];

		// Compute 1/4 of RT2*Matrix(error-vector)*R1
		rmatBufferA[0] = rmatBufferA[4] = rmatBufferA[8] = 0;
		rmatBufferA[7] =  magAlignmentError[0];
		rmatBufferA[5] = -magAlignmentError[0];
		rmatBufferA[2] =  magAlignmentError[1];
		rmatBufferA[6] = -magAlignmentError[1];
		rmatBufferA[3] =  magAlignmentError[2];
		rmatBufferA[1] = -magAlignmentError[2];
		MatrixMultiply(3, 3, 3, rmatBufferB, rmatBufferA, rmatDelayCompensated);
		MatrixMultiply(3, 3, 3, rmatBufferA, rmat2Transpose, rmatBufferB);

		// taking advantage of factor of 1/4 in the two matrix multiplies, compute
		// 1/2 of the vector representation of the rotation
		R2TAlignmentErrorR1[0] = (rmatBufferA[7] - rmatBufferA[5]);
		R2TAlignmentErrorR1[1] = (rmatBufferA[2] - rmatBufferA[6]);
		R2TAlignmentErrorR1[2] = (rmatBufferA[3] - rmatBufferA[1]);

		// compute the negative of estimate of the residual misalignment
		VectorCross(magAlignmentAdjustment, R2TAlignmentErrorR1, R2TR1RotationVector);

		if (dcm_flags._.first_mag_reading == 0)
		{
			udb_magOffset[0] = udb_magOffset[0] + ((offsetEstimate[0] + 2) >> 2);
			udb_magOffset[1] = udb_magOffset[1] + ((offsetEstimate[1] + 2) >> 2);
			udb_magOffset[2] = udb_magOffset[2] + ((offsetEstimate[2] + 2) >> 2);
			quaternion_adjust(magAlignment, magAlignmentAdjustment);
		}
		else
		{
			dcm_flags._.first_mag_reading = 0;
		}

		VectorCopy(3, magFieldEarthNormalizedPrevious, magFieldEarthNormalized);
		VectorCopy(9, rmatPrevious, rmatDelayCompensated);

		dcm_flags._.mag_drift_req = 0;
	}
}

void mag_drift_callback(void)
{
	dcm_flags._.mag_drift_req = 1;
//#define USE_DEBUG_IO
#ifdef USE_DEBUG_IO
//	printf("magno %u %u %u\r\n", udb_magFieldBody[0], udb_magFieldBody[1], udb_magFieldBody[2]);
#endif
}

#endif // MAG_YAW_DRIFT
