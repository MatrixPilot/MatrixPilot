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

#include "rmat_obj.h"

//TODO: accel. comp. disabled
// disable GPS-based acceleration compensation

boolean gps_nav_valid_obj(void) {
    return false;
};


//		These are the routines for maintaining a direction cosine matrix
//		that can be used to transform vectors between the earth and plane
//		coordinate systems. The 9 direction cosines in the matrix completely
//		define the orientation of the plane with respect to the earth.
//		The inverse of the matrix is equal to its transpose. This defines
//		the so-called orthogonality conditions, which impose 6 constraints on
//		the 9 elements of the matrix.

//	All numbers are stored in 2.14 format.
//	Vector and matrix libraries work in 1.15 format.
//	This combination allows values of matrix elements between -2 and +2.
//	Multiplication produces results scaled by 1/2.

void diag3x3(fractional *matrix, fractional a, fractional b, fractional c) {
    matrix[0] = a;
    matrix[1] = 0;
    matrix[2] = 0;
    matrix[3] = 0;
    matrix[4] = b;
    matrix[5] = 0;
    matrix[6] = 0;
    matrix[7] = 0;
    matrix[8] = c;
}

void zero3x1(fractional *vector) {
    vector[0] = 0;
    vector[1] = 0;
    vector[2] = 0;
}

void zero3x1_long(long *vector) {
    vector[0] = 0;
    vector[1] = 0;
    vector[2] = 0;
}

void init3x1(fractional *vector, fractional a, fractional b, fractional c) {
    vector[0] = a;
    vector[1] = b;
    vector[2] = c;
}

struct IMUstate mpuState;

void dcm_init_rmat_obj(void) {
#if ( BOARD_TYPE == UDB3_BOARD )
    //Paul's gains corrected for GGAIN
    mpuState.KProllpitch = 256 * 5;
    mpuState.KIrollpitch = 256;

#elif ( BOARD_TYPE == AUAV2_BOARD_ALPHA1 )
    // modified gains for AUAV2/MPU6000
    mpuState.KProllpitch = (ACCEL_RANGE * 1280 / 3);
    mpuState.KIrollpitch = (ACCEL_RANGE * 3400 / HEARTBEAT_HZ);

#elif ( BOARD_TYPE == UDB4_BOARD )
    //Paul's gains for 6G accelerometers
    mpuState.KProllpitch = 256 * 10;
    mpuState.KIrollpitch = (20400 / HEARTBEAT_HZ);

#else
#error Unsupported BOARD_TYPE
#endif

    mpuState.KPyaw = 256 * 4;
    mpuState.KIyaw = (1280 / HEARTBEAT_HZ);

    mpuState.Scale_gyro = 2.999;
    mpuState.Scale_accel = 2.578 * ACCEL_RANGE / 8;

    // gravity in AtoD/2 units
    mpuState.Gravity = ((long) (5280.0 / mpuState.Scale_accel));

    // one degree per second, in AtoD/2 units
    mpuState.DegPerSec =  98.3 / mpuState.Scale_gyro;

    //	integration multiplier for gyros
    mpuState.G_gain = mpuState.Scale_gyro * 6 * (RMAX * (1.0 / HEARTBEAT_HZ));

    mpuState.ggain[0] = mpuState.G_gain;
    mpuState.ggain[1] = mpuState.G_gain;
    mpuState.ggain[2] = mpuState.G_gain;

    mpuState.spin_axis[0] = 0;
    mpuState.spin_axis[1] = 0;
    mpuState.spin_axis[2] = RMAX;

    // threshold at which gyros may be saturated
    mpuState.Gyro_sat = 15000;

    //	rmat is the matrix of direction cosines relating
    //	the body and earth coordinate systems.
    //	The columns of rmat are the axis vectors of the plane,
    //	as measured in the earth reference frame.
    //	rmat is initialized to the identity matrix in 2.14 fractional format
    diag3x3(mpuState.rmat, RMAX, RMAX, RMAX);
    diag3x3(mpuState.rmatDelayCompensated, RMAX, RMAX, RMAX);

    //	gyro rotation vector:
    zero3x1(mpuState.omegagyro);
    zero3x1(mpuState.omega);

    //	gyro correction vectors:
    zero3x1(mpuState.omegacorrP);
    zero3x1(mpuState.omegacorrI);

    //  acceleration, as measured in GPS earth coordinate system
    zero3x1(mpuState.accelEarth);

    //    mpuState.accelEarthFiltered[0].WW = 0;
    //    mpuState.accelEarthFiltered[1].WW = 0;
    //    mpuState.accelEarthFiltered[2].WW = 0;

    //	correction vector integrators ;
    mpuState.gyroCorrectionIntegral[0].WW = 0;
    mpuState.gyroCorrectionIntegral[1].WW = 0;
    mpuState.gyroCorrectionIntegral[2].WW = 0;

    //	accumulator for computing adjusted omega:
    zero3x1(mpuState.omegaAccum);

    //	gravity, as measured in plane coordinate system
    init3x1(mpuState.gplane, 0, 0, mpuState.Gravity);

    //	horizontal velocity over ground, as measured by GPS (Vz = 0 )
    init3x1(mpuState.dirovergndHGPS, 0, RMAX, 0);

    //	horizontal direction over ground, as indicated by Rmatrix
    init3x1(mpuState.dirovergndHRmat, 0, RMAX, 0);

    //	rotation angle equal to omega times integration factor:
    //  fractional theta[] = { 0 , 0 , 0 } ;

    //	matrix buffer:
    //  fractional rbuff[] = { 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 } ;

    zero3x1_long(mpuState.accelerometer_earth_integral);
    zero3x1(mpuState.GPS_velocity_previous);
    mpuState.accelerometer_samples = MAX_ACCEL_SAMPLES + 1;

    //	vector buffer
    zero3x1(mpuState.errorRP);
    zero3x1(mpuState.errorYawground);
    zero3x1(mpuState.errorYawplane);

    //	measure of error in orthogonality, used for debugging purposes:
    mpuState.error = 0;

#if ( MAG_YAW_DRIFT == 1 )
#if (DECLINATIONANGLE_VARIABLE == 1)
    mpuState.dcm_declination_angle.BB = DECLINATIONANGLE;
#endif
    mpuState.declinationVector[0] = cosine((signed char) (DECLINATIONANGLE >> 8));
    mpuState.declinationVector[1] = sine((signed char) (DECLINATIONANGLE >> 8));
#endif
    // initialize boxCar filter state
    init_boxCarState(ACC_BOX_LEN, ACC_BOX_N, mpuState.boxCarBuff, mpuState.boxCarSum, &mpuState.filterState);
}

//	Implement the cross product. *dest = *src1X*src2 ;

void VectorCross_obj(fractional * dest, fractional * src1, fractional * src2) {
    union longww crossaccum;
    crossaccum.WW = __builtin_mulss(src1[1], src2[2]);
    crossaccum.WW -= __builtin_mulss(src1[2], src2[1]);
    crossaccum.WW *= 4;
    dest[0] = crossaccum._.W1;
    crossaccum.WW = __builtin_mulss(src1[2], src2[0]);
    crossaccum.WW -= __builtin_mulss(src1[0], src2[2]);
    crossaccum.WW *= 4;
    dest[1] = crossaccum._.W1;
    crossaccum.WW = __builtin_mulss(src1[0], src2[1]);
    crossaccum.WW -= __builtin_mulss(src1[1], src2[0]);
    crossaccum.WW *= 4;
    dest[2] = crossaccum._.W1;
    return;
}

void read_gyros_obj()
//	fetch the gyro signals and subtract the baseline offset,
//	and adjust for variations in supply voltage
{
    unsigned spin_rate_over_2;
#if ( HILSIM == 1 )
    mpuState.omegagyro[0] = q_sim.BB;
    mpuState.omegagyro[1] = p_sim.BB;
    mpuState.omegagyro[2] = r_sim.BB;
#else
    mpuState.omegagyro[0] = XRATE_MPU;
    mpuState.omegagyro[1] = YRATE_MPU;
    mpuState.omegagyro[2] = ZRATE_MPU;
#endif

    mpuState.spin_rate = vector3_mag(mpuState.omegagyro[0], mpuState.omegagyro[1], mpuState.omegagyro[2]);
    spin_rate_over_2 = mpuState.spin_rate >> 1;

    if (spin_rate_over_2 > 0) {
        mpuState.spin_axis[0] = __builtin_divsd(((long) mpuState.omegagyro[0]) << 13, spin_rate_over_2);
        mpuState.spin_axis[1] = __builtin_divsd(((long) mpuState.omegagyro[1]) << 13, spin_rate_over_2);
        mpuState.spin_axis[2] = __builtin_divsd(((long) mpuState.omegagyro[2]) << 13, spin_rate_over_2);
    }

    return;
}

void read_accel_obj() {
#if ( HILSIM == 1 )
    mpuState.gplane[0] = v_dot_sim.BB;
    mpuState.gplane[1] = u_dot_sim.BB;
    mpuState.gplane[2] = w_dot_sim.BB;
#else
    mpuState.gplane[0] = XACCEL_MPU;
    mpuState.gplane[1] = YACCEL_MPU;
    mpuState.gplane[2] = ZACCEL_MPU;
#endif

    mpuState.accelEarth[0] = VectorDotProduct(3, &mpuState.rmat[0], mpuState.gplane) << 1;
    mpuState.accelEarth[1] = -VectorDotProduct(3, &mpuState.rmat[3], mpuState.gplane) << 1;
    mpuState.accelEarth[2] = -((int) mpuState.Gravity) + (VectorDotProduct(3, &mpuState.rmat[6], mpuState.gplane) << 1);

    //	mpuState.accelEarthFiltered[0].WW += ((((long)mpuState.accelEarth[0])<<16) - mpuState.accelEarthFiltered[0].WW)>>5 ;
    //	mpuState.accelEarthFiltered[1].WW += ((((long)mpuState.accelEarth[1])<<16) - mpuState.accelEarthFiltered[1].WW)>>5 ;
    //	mpuState.accelEarthFiltered[2].WW += ((((long)mpuState.accelEarth[2])<<16) - mpuState.accelEarthFiltered[2].WW)>>5 ;

    return;
}

//	multiplies omega times speed, and scales appropriately
//  omega in radians per second, speed in cm per second
//TODO: this method is never called
//TODO: fix this for dual IMUs

int omegaSOG_obj(int omega, unsigned int speed) {
    union longww working;
    speed = speed >> 3;
    working.WW = __builtin_mulsu(omega, speed);
    if (((int) working._.W1)> ((int) CENTRIFSAT)) {
        return RMAX;
    } else if (((int) working._.W1) < ((int) -CENTRIFSAT)) {
        return -RMAX;
    } else {
        working.WW = working.WW >> 5;
        working.WW = __builtin_mulsu(working._.W0, CENTRISCALE);
        working.WW = working.WW << 5;
        return working._.W1;
    }
}

/*	WJP
void adj_accel()
{
        // total (3D) airspeed in cm/sec is used to adjust for acceleration
        // airspeed and fwd accel are both zero without GPS
        mpuState.gplane[0]=mpuState.gplane[0]- omegaSOG( mpuState.omegaAccum[2] , air_speed_3DGPS ) ;
        mpuState.gplane[2]=mpuState.gplane[2]+ omegaSOG( mpuState.omegaAccum[0] , air_speed_3DGPS ) ;
        mpuState.gplane[1]=mpuState.gplane[1]+ ((unsigned int)(ACCELSCALE))*forward_acceleration ;

        return ;
}
 */

//	The update algorithm!!

void rupdate_obj(void)
//	This is the key routine. It performs a small rotation
//	on the direction cosine matrix, based on the gyro vector and correction.
//	It uses vector and matrix routines furnished by Microchip.
{
    fractional rup[9];
    //	fractional theta[3] ;
    fractional rbuff[9];
    unsigned long thetaSquare;
    unsigned nonlinearAdjust;

    VectorAdd(3, mpuState.omegaAccum, mpuState.omegagyro, mpuState.omegacorrI);
    VectorAdd(3, mpuState.omega, mpuState.omegaAccum, mpuState.omegacorrP);
    //	scale by the integration factors:
    VectorMultiply(3, mpuState.theta, mpuState.omega, mpuState.ggain); // Scalegain of 2
    //FIXME: diagonal elements of the update matrix:
    rup[0] = rup[4] = rup[8] = RMAX;

    // compute the square of rotation

    thetaSquare = __builtin_mulss(mpuState.theta[0], mpuState.theta[0]) +
            __builtin_mulss(mpuState.theta[1], mpuState.theta[1]) +
            __builtin_mulss(mpuState.theta[2], mpuState.theta[2]);

    // adjust gain by rotation_squared divided by 3

    nonlinearAdjust = RMAX + ((unsigned int) (thetaSquare >> 14)) / 3;

    mpuState.theta[0] = __builtin_mulsu(mpuState.theta[0], nonlinearAdjust) >> 14;
    mpuState.theta[1] = __builtin_mulsu(mpuState.theta[1], nonlinearAdjust) >> 14;
    mpuState.theta[2] = __builtin_mulsu(mpuState.theta[2], nonlinearAdjust) >> 14;

    //	construct the off-diagonal elements of the update matrix:
    rup[1] = -mpuState.theta[2];
    rup[2] = mpuState.theta[1];
    rup[3] = mpuState.theta[2];
    rup[5] = -mpuState.theta[0];
    rup[6] = -mpuState.theta[1];
    rup[7] = mpuState.theta[0];

    //	matrix multiply the rmatrix by the update matrix
    MatrixMultiply(3, 3, 3, rbuff, mpuState.rmat, rup);
    //	multiply by 2 and copy back from rbuff to rmat:
    MatrixAdd(3, 3, mpuState.rmat, rbuff, rbuff);
    return;
}

//	normalization algorithm:

void normalize_obj(void)
//	This is the routine that maintains the orthogonality of the
//	direction cosine matrix, which is expressed by the identity
//	relationship that the cosine matrix multiplied by its
//	transpose should equal the identity matrix.
//	Small adjustments are made at each time step to assure orthogonality.
{
    fractional norm; // actual magnitude
    fractional renorm; // renormalization factor
    fractional rbuff[9];
    //	compute -1/2 of the dot product between rows 1 and 2
    mpuState.error = -VectorDotProduct(3, &mpuState.rmat[0], &mpuState.rmat[3]); // note, 1/2 is built into 2.14
    //	scale rows 1 and 2 by the mpuState.error
    VectorScale(3, &rbuff[0], &mpuState.rmat[3], mpuState.error);
    VectorScale(3, &rbuff[3], &mpuState.rmat[0], mpuState.error);
    //	update the first 2 rows to make them closer to orthogonal:
    VectorAdd(3, &rbuff[0], &rbuff[0], &mpuState.rmat[0]);
    VectorAdd(3, &rbuff[3], &rbuff[3], &mpuState.rmat[3]);
    //	use the cross product of the first 2 rows to get the 3rd row
    VectorCross_obj(&rbuff[6], &rbuff[0], &rbuff[3]);


    //	Use a Taylor's expansion for 1/sqrt(X*X) to avoid division in the renormalization
    //	rescale row1
    norm = VectorPower(3, &rbuff[0]); // Scalegain of 0.5
    renorm = RMAX15 - norm;
    VectorScale(3, &rbuff[0], &rbuff[0], renorm);
    VectorAdd(3, &mpuState.rmat[0], &rbuff[0], &rbuff[0]);
    //	rescale row2
    norm = VectorPower(3, &rbuff[3]);
    renorm = RMAX15 - norm;
    VectorScale(3, &rbuff[3], &rbuff[3], renorm);
    VectorAdd(3, &mpuState.rmat[3], &rbuff[3], &rbuff[3]);
    //	rescale row3
    norm = VectorPower(3, &rbuff[6]);
    renorm = RMAX15 - norm;
    VectorScale(3, &rbuff[6], &rbuff[6], renorm);
    VectorAdd(3, &mpuState.rmat[6], &rbuff[6], &rbuff[6]);
    return;
}

#if GPS_ACCEL_COMP != 0

void roll_pitch_drift_obj() {
    VectorCross_obj(errorRP, mpuState.gplane, &mpuState.rmat[6]);
    return;
}
#else

void roll_pitch_drift_obj() {

    //    int accelerometer_earth[3];
    //    int GPS_acceleration[3];
    //    int accelerometer_reference[3];
    //    int errorRP_earth[3];

    //	integrate the accelerometer signals in earth frame of reference
    // using a 20 sample boxcar filter (100msec with 200Hz heartbeat)
    boxcar(mpuState.gplane, &mpuState.filterState, mpuState.gplaneFilt);

    // rotate acceleration vector into earth frame
    mpuState.accelerometer_earth[0] = (VectorDotProduct(3, &mpuState.rmat[0], mpuState.gplaneFilt) << 1);
    mpuState.accelerometer_earth[1] = (VectorDotProduct(3, &mpuState.rmat[3], mpuState.gplaneFilt) << 1);
    mpuState.accelerometer_earth[2] = (VectorDotProduct(3, &mpuState.rmat[6], mpuState.gplaneFilt) << 1);
    //    accelerometer_samples++;

    //	if there is GPS information available, or if GPS is offline and there are enough samples, compute the roll-pitch correction
    if ((dcm_flags._.rollpitch_req == 1) || (mpuState.accelerometer_samples > MAX_ACCEL_SAMPLES)) {
        //        if (mpuState.accelerometer_samples > 0)
        //        {
        //            // compute the average of the integral
        //            mpuState.accelerometer_earth[0] = __builtin_divsd(mpuState.accelerometer_earth_integral[0], mpuState.accelerometer_samples);
        //            mpuState.accelerometer_earth[1] = __builtin_divsd(mpuState.accelerometer_earth_integral[1], mpuState.accelerometer_samples);
        //            mpuState.accelerometer_earth[2] = __builtin_divsd(mpuState.accelerometer_earth_integral[2], mpuState.accelerometer_samples);
        //        }
        //        else
        //        {
        //            mpuState.accelerometer_earth[0] = mpuState.accelerometer_earth[1] = 0;
        //            mpuState.accelerometer_earth[2] = mpuState.Gravity;
        //        }

        if ((HILSIM == 1) || (!gps_nav_valid_obj()) || (NEW_RP_DRIFT == 0)) {
            // cannot do acceleration compensation, assume no acceleration
            mpuState.accelerometer_reference[0] = mpuState.accelerometer_reference[1] = 0;
            mpuState.accelerometer_reference[2] = RMAX;
        } else {
            if (dcm_flags._.rollpitch_req == 1) {
                mpuState.GPS_acceleration[0] = __builtin_divsd(__builtin_mulsu((GPSvelocity.x - mpuState.GPS_velocity_previous[0]), ACCEL_SAMPLES_PER_SEC), mpuState.accelerometer_samples);
                mpuState.GPS_acceleration[1] = __builtin_divsd(__builtin_mulsu((GPSvelocity.y - mpuState.GPS_velocity_previous[1]), ACCEL_SAMPLES_PER_SEC), mpuState.accelerometer_samples);
                mpuState.GPS_acceleration[2] = __builtin_divsd(__builtin_mulsu((GPSvelocity.z - mpuState.GPS_velocity_previous[2]), ACCEL_SAMPLES_PER_SEC), mpuState.accelerometer_samples);

                mpuState.GPS_velocity_previous[0] = GPSvelocity.x;
                mpuState.GPS_velocity_previous[1] = GPSvelocity.y;
                mpuState.GPS_velocity_previous[2] = GPSvelocity.z;
            } else {
                mpuState.GPS_acceleration[0] = 0;
                mpuState.GPS_acceleration[1] = 0;
                mpuState.GPS_acceleration[2] = 0;
            }

            // acceleration_reference = normalize ( gravity_earth - 40* delta_GPS_velocity/samples )

            mpuState.accelerometer_reference[0] = mpuState.GPS_acceleration[0]; // GPS x is opposite sign to DCM x
            mpuState.accelerometer_reference[1] = -mpuState.GPS_acceleration[1]; // GPS y is same sign as DCM y
            mpuState.accelerometer_reference[2] = 981 + mpuState.GPS_acceleration[2]; // gravity is 981 centimeters/sec/sec, z sign is opposite

            vector3_normalize(mpuState.accelerometer_reference, mpuState.accelerometer_reference);
        }

        //	error_earth = accelerometer_earth cross accelerometer_reference, set error_earth[2] = 0 ;
        VectorCross_obj(mpuState.errorRP_earth, mpuState.accelerometer_earth, mpuState.accelerometer_reference);
        mpuState.errorRP_earth[2] = 0;

        //	error_body = Rtranspose * error_earth

        //	*** Note: this accomplishes multiplication rmat transpose times errorRP_earth!!
        MatrixMultiply(1, 3, 3, mpuState.errorRP, mpuState.errorRP_earth, mpuState.rmat);

        mpuState.accelerometer_earth_integral[0] = 0;
        mpuState.accelerometer_earth_integral[1] = 0;
        mpuState.accelerometer_earth_integral[2] = 0;

        //        mpuState.accelerometer_samples = 0;
        dcm_flags._.rollpitch_req = 0;
    }
    return;
}
#endif

/*
void yaw_drift()
{
    //	although yaw correction is done in horizontal plane,
    //	this is done in 3 dimensions, just in case we change our minds later
    //	form the horizontal direction over ground based on rmat
    if (dcm_flags._.yaw_req)
    {
        if (ground_velocity_magnitudeXY > GPS_SPEED_MIN)
        {
            //	vector cross product to get the rotation error in ground frame
            VectorCross(mpuState.errorYawground, mpuState.dirovergndHRmat, mpuState.dirovergndHGPS);
            //	convert to plane frame:
            //	*** Note: this accomplishes multiplication rmat transpose times errorYawground!!
            MatrixMultiply(1, 3, 3, mpuState.errorYawplane, mpuState.errorYawground, mpuState.rmat);
        }
        else
        {
            mpuState.errorYawplane[0] = 0;
            mpuState.errorYawplane[1] = 0;
            mpuState.errorYawplane[2] = 0;
        }

        dcm_flags._.yaw_req = 0;
    }
    return;
}
 */

#if (MAG_YAW_DRIFT == 1)

fractional magFieldEarth[3];
extern fractional udb_magFieldBody[3];
extern fractional udb_magOffset[3];
fractional rmatPrevious[9];
fractional magFieldEarthNormalizedPrevious[3];
fractional magAlignment[4] = {0, 0, 0, RMAX};
fractional magFieldBodyMagnitudePrevious;
fractional magFieldBodyPrevious[3];

//fractional rmatPrevious[9];

//int offsetDelta[3] ;

void align_rmat_to_mag(void) {
    unsigned char theta;
    struct relative2D initialBodyField;
    int costheta;
    int sintheta;
    initialBodyField.x = udb_magFieldBody[0];
    initialBodyField.y = udb_magFieldBody[1];
#if(DECLINATIONANGLE_VARIABLE == 1)
    theta = rect_to_polar(&initialBodyField) - 64 - (dcm_declination_angle._.B1);
#else
    theta = rect_to_polar(&initialBodyField) - 64 - (DECLINATIONANGLE >> 8);
#endif
    costheta = cosine(theta);
    sintheta = sine(theta);
    mpuState.rmat[0] = mpuState.rmat[4] = costheta;
    mpuState.rmat[1] = sintheta;
    mpuState.rmat[3] = -sintheta;
    return;
}

void quaternion_adjust(fractional quaternion[], fractional direction[]) {
    //	performs an adjustment to a quaternion representation of re-alignement.
    //	the cross product is left out, theory and test both show it should not be used.
    fractional delta_cos;
    fractional vector_buffer[3];
    fractional increment[3];
    unsigned long int magnitudesqr;
    unsigned magnitude;
    increment[0] = direction[0] >> 3;
    increment[1] = direction[1] >> 3;
    increment[2] = direction[2] >> 3;
    // change is cosine is 1/2 of the dot product of first 3 elements of quaternion
    // with the increment. The 1/2 is built into the dot product.
    delta_cos = -VectorDotProduct(3, quaternion, increment);
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

    quaternion[0] = __builtin_divsd(__builtin_mulsu(quaternion[0], RMAX), magnitude);
    quaternion[1] = __builtin_divsd(__builtin_mulsu(quaternion[1], RMAX), magnitude);
    quaternion[2] = __builtin_divsd(__builtin_mulsu(quaternion[2], RMAX), magnitude);
    quaternion[3] = __builtin_divsd(__builtin_mulsu(quaternion[3], RMAX), magnitude);

    return;
}

void RotVector2RotMat(fractional rotation_matrix[], fractional rotation_vector[]) {
    //	rotation vector represents a rotation in vector form
    //	around an axis equal to the normalized value of the vector.
    //	It is assumed that rotation_vector already includes a factor of sin(alpha/2)
    //  maximum rotation is plus minus 180 degrees.
    fractional cos_alpha;
    fractional cos_half_alpha;
    fractional cos_half_alpha_rotation_vector[3];
    union longww sin_half_alpha_sqr = {0};
    int matrix_index;

    cos_half_alpha = rotation_vector[3];

    //	compute the square of sine of half alpha
    for (matrix_index = 0; matrix_index < 3; matrix_index++) {
        sin_half_alpha_sqr.WW += __builtin_mulss(rotation_vector[matrix_index], rotation_vector[matrix_index]);
    }
    if (sin_half_alpha_sqr.WW > ((long) RMAX * RMAX - 1)) {
        sin_half_alpha_sqr.WW = (long) RMAX * RMAX - 1;
    }

    //	compute cos_alpha
    sin_half_alpha_sqr.WW *= 8;
    cos_alpha = RMAX - sin_half_alpha_sqr._.W1;

    //	scale rotation_vector by 2*cos_half_alpha
    VectorScale(3, cos_half_alpha_rotation_vector, rotation_vector, cos_half_alpha);
    for (matrix_index = 0; matrix_index < 3; matrix_index++) {
        cos_half_alpha_rotation_vector[matrix_index] *= 4;
    }

    //	compute 2 times rotation_vector times its transpose
    MatrixMultiply(3, 1, 3, rotation_matrix, rotation_vector, rotation_vector);
    for (matrix_index = 0; matrix_index < 9; matrix_index++) {
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

    return;
}

#define MAG_LATENCY 0.085 // seconds
#define MAG_LATENCY_COUNT ( ( int ) ( HEARTBEAT_HZ * MAG_LATENCY ) )

// Since mag_drift is called every heartbeat the first assignment to rmatDelayCompensated
// will occur at udb_heartbeat_counter = (.25 - MAG_LATENCY) seconds.
// Since rxMagnetometer is called  at multiples of .25 seconds, this initial
// delay offsets the 4Hz updates of rmatDelayCompensated by MAG_LATENCY seconds.
int mag_latency_counter = (HEARTBEAT_HZ / 4) - MAG_LATENCY_COUNT;

void mag_drift() {
    int mag_error;
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

    // the following compensates for magnetometer latency by adjusting the timing
    // of when rmat is read
    mag_latency_counter--;
    if (mag_latency_counter == 0) {
        mpuState.rmatDelayTime = uptime;
        // uptime is bumped by a level 6 ISR, but we're called at level 3
        // from dcm_run_imu_step via udb_servo_callback_prepare_outputs via _T6Interrupt (IPL 3)
        // given the synchronous nature of this interrupt, (_THEARTBEATIF=_T6IF is set by the
        // level6 ISR _T1Interrupt) we'll never see uptime change inside mag_drift unless
        // some higher priority ISR burns a significant percentage of cpu time
        VectorCopy(9, mpuState.rmatDelayCompensated, mpuState.rmat);
        mag_latency_counter = (HEARTBEAT_HZ / 4); // not really needed, but its good insurance
        // mag_latency_counter is assigned in the next block
    }

    if (dcm_flags._.mag_drift_req) {

        //		Compute magnetic offsets
        magFieldBodyMagnitude = vector3_mag(udb_magFieldBody[0], udb_magFieldBody[1], udb_magFieldBody[2]);
        VectorSubtract(3, vectorBuffer, udb_magFieldBody, magFieldBodyPrevious);
        vector3_normalize(vectorBuffer, vectorBuffer);
        VectorScale(3, offsetEstimate, vectorBuffer, magFieldBodyMagnitude - magFieldBodyMagnitudePrevious);
        VectorCopy(3, magFieldBodyPrevious, udb_magFieldBody);
        magFieldBodyMagnitudePrevious = magFieldBodyMagnitude;

        //		Compute and apply the magnetometer alignment adjustment in the body frame
        RotVector2RotMat(rmatBufferA, magAlignment);
        vectorBuffer[0] = VectorDotProduct(3, &rmatBufferA[0], udb_magFieldBody) << 1;
        vectorBuffer[1] = VectorDotProduct(3, &rmatBufferA[3], udb_magFieldBody) << 1;
        vectorBuffer[2] = VectorDotProduct(3, &rmatBufferA[6], udb_magFieldBody) << 1;
        VectorCopy(3, udb_magFieldBody, vectorBuffer);

        if (dcm_flags._.first_mag_reading == 1) {
            align_rmat_to_mag();
            mpuState.rmatDelayTime = uptime;
            VectorCopy(9, mpuState.rmatDelayCompensated, mpuState.rmat);
        }

        mag_latency_counter = (HEARTBEAT_HZ / 4) - MAG_LATENCY_COUNT; // setup for the next reading

        //		Compute the mag field in the earth frame
        magFieldEarth[0] = VectorDotProduct(3, &mpuState.rmatDelayCompensated[0], udb_magFieldBody) << 1;
        magFieldEarth[1] = VectorDotProduct(3, &mpuState.rmatDelayCompensated[3], udb_magFieldBody) << 1;
        magFieldEarth[2] = VectorDotProduct(3, &mpuState.rmatDelayCompensated[6], udb_magFieldBody) << 1;

        //		Normalize the magnetic vector to RMAT

        vector3_normalize(magFieldEarthNormalized, magFieldEarth);
        vector2_normalize(magFieldEarthHorzNorm, magFieldEarth);

        //		Use the magnetometer to detect yaw drift

#if(DECLINATIONANGLE_VARIABLE == 1)
        mpuState.declinationVector[0] = cosine(dcm_declination_angle._.B1);
        mpuState.declinationVector[1] = sine(dcm_declination_angle._.B1);
#endif
        mag_error = VectorDotProduct(2, magFieldEarthHorzNorm, mpuState.declinationVector);
        VectorScale(3, mpuState.errorYawplane, &mpuState.rmat[6], mag_error); // Scalegain = 1/2

        //		Do the computations needed to compensate for magnetometer misalignment

        //		Determine the apparent shift in the earth's magnetic field:
        VectorCross_obj(magAlignmentError, magFieldEarthNormalizedPrevious, magFieldEarthNormalized);

        //		Compute R2 transpose
        MatrixTranspose(3, 3, rmat2Transpose, mpuState.rmatDelayCompensated);

        //		Compute 1/2 of R2tranpose times R1
        MatrixMultiply(3, 3, 3, rmatBufferA, rmat2Transpose, rmatPrevious);

        //		Convert to a rotation vector, take advantage of 1/2 from the previous step
        R2TR1RotationVector[0] = rmatBufferA[7] - rmatBufferA[5];
        R2TR1RotationVector[1] = rmatBufferA[2] - rmatBufferA[6];
        R2TR1RotationVector[2] = rmatBufferA[3] - rmatBufferA[1];

        //		Compute 1/4 of RT2*Matrix(error-vector)*R1
        rmatBufferA[0] = rmatBufferA[4] = rmatBufferA[8] = 0;
        rmatBufferA[7] = magAlignmentError[0];
        rmatBufferA[5] = -magAlignmentError[0];
        rmatBufferA[2] = magAlignmentError[1];
        rmatBufferA[6] = -magAlignmentError[1];
        rmatBufferA[3] = magAlignmentError[2];
        rmatBufferA[1] = -magAlignmentError[2];
        MatrixMultiply(3, 3, 3, rmatBufferB, rmatBufferA, mpuState.rmatDelayCompensated);
        MatrixMultiply(3, 3, 3, rmatBufferA, rmat2Transpose, rmatBufferB);

        //		taking advantage of factor of 1/4 in the two matrix multiplies, compute
        //		1/2 of the vector representation of the rotation
        R2TAlignmentErrorR1[0] = (rmatBufferA[7] - rmatBufferA[5]);
        R2TAlignmentErrorR1[1] = (rmatBufferA[2] - rmatBufferA[6]);
        R2TAlignmentErrorR1[2] = (rmatBufferA[3] - rmatBufferA[1]);

        //		compute the negative of estimate of the residual misalignment
        VectorCross_obj(magAlignmentAdjustment, R2TAlignmentErrorR1, R2TR1RotationVector);

        if (dcm_flags._.first_mag_reading == 0) {

#ifdef ENABLE_MAGALIGNMENT
            udb_magOffset[0] = udb_magOffset[0] + ((offsetEstimate[0] + 2) >> 2);
            udb_magOffset[1] = udb_magOffset[1] + ((offsetEstimate[1] + 2) >> 2);
            udb_magOffset[2] = udb_magOffset[2] + ((offsetEstimate[2] + 2) >> 2);

            quaternion_adjust(magAlignment, magAlignmentAdjustment);
#endif

        } else {
            dcm_flags._.first_mag_reading = 0;
        }
        VectorCopy(3, magFieldEarthNormalizedPrevious, magFieldEarthNormalized);
        VectorCopy(9, rmatPrevious, mpuState.rmatDelayCompensated);

        dcm_flags._.mag_drift_req = 0;
    }
    return;
}

#endif

#define MAXIMUM_SPIN_DCM_INTEGRAL 20.0 // degrees per second

void PI_feedback_obj(void) {
    fractional errorRPScaled[3];
    int kpyaw;
    int kprollpitch;

    // boost the KPs at high spin rate, to compensate for increased error due to calibration error
    // above 50 degrees/second, scale by rotation rate divided by 50

    if (mpuState.spin_rate < ((unsigned int) (50.0 * mpuState.DegPerSec))) {
        kpyaw = mpuState.KPyaw;
        kprollpitch = mpuState.KProllpitch;
    } else if (mpuState.spin_rate < ((unsigned int) (500.0 * mpuState.DegPerSec))) {
        kpyaw = ((unsigned int) (mpuState.KPyaw * 8.0 / (50.0 * mpuState.DegPerSec)))*(mpuState.spin_rate >> 3);
        kprollpitch = ((unsigned int) (mpuState.KProllpitch * 8.0 / (50.0 * mpuState.DegPerSec)))*(mpuState.spin_rate >> 3);
    } else {
        kpyaw = (int) (10.0 * mpuState.KPyaw);
        kprollpitch = (int) (10.0 * mpuState.KProllpitch);
    }

#if (ENABLE_MAG_DRIFT_CORRECTION == 1)
    VectorScale(3, mpuState.omegacorrP, mpuState.errorYawplane, kpyaw); // Scale gain = 2
#else
    mpuState.omegacorrP[0] = 0;
    mpuState.omegacorrP[1] = 0;
    mpuState.omegacorrP[2] = 0;
#endif
    VectorScale(3, errorRPScaled, mpuState.errorRP, kprollpitch); // Scale gain = 2
    VectorAdd(3, mpuState.omegacorrP, mpuState.omegacorrP, errorRPScaled);

    // turn off the offset integrator while spinning, it doesn't work in that case,
    // and it only causes trouble.

    if (mpuState.spin_rate < ((unsigned int) (MAXIMUM_SPIN_DCM_INTEGRAL * mpuState.DegPerSec))) {
        mpuState.gyroCorrectionIntegral[0].WW += (__builtin_mulss(mpuState.errorRP[0], mpuState.KIrollpitch) >> 3);
        mpuState.gyroCorrectionIntegral[1].WW += (__builtin_mulss(mpuState.errorRP[1], mpuState.KIrollpitch) >> 3);
        mpuState.gyroCorrectionIntegral[2].WW += (__builtin_mulss(mpuState.errorRP[2], mpuState.KIrollpitch) >> 3);
#if (ENABLE_MAG_DRIFT_CORRECTION == 1)
        mpuState.gyroCorrectionIntegral[0].WW += (__builtin_mulss(mpuState.errorYawplane[0], mpuState.KIyaw) >> 3);
        mpuState.gyroCorrectionIntegral[1].WW += (__builtin_mulss(mpuState.errorYawplane[1], mpuState.KIyaw) >> 3);
        mpuState.gyroCorrectionIntegral[2].WW += (__builtin_mulss(mpuState.errorYawplane[2], mpuState.KIyaw) >> 3);
#endif
    }

    mpuState.omegacorrI[0] = mpuState.gyroCorrectionIntegral[0]._.W1 >> 3;
    mpuState.omegacorrI[1] = mpuState.gyroCorrectionIntegral[1]._.W1 >> 3;
    mpuState.omegacorrI[2] = mpuState.gyroCorrectionIntegral[2]._.W1 >> 3;

    return;
}

unsigned int adjust_gyro_gain_obj(unsigned int old_gain, int gain_change) {
    unsigned int gain;
    gain = old_gain + gain_change;
    if (gain > (unsigned int) (1.1 * mpuState.G_gain)) {
        gain = (unsigned int) (1.1 * mpuState.G_gain);
    }
    if (gain < (unsigned int) (0.9 * mpuState.G_gain)) {
        gain = (unsigned int) (0.9 * mpuState.G_gain);
    }
    return gain;
}

#define GYRO_CALIB_TAU 10.0
#define MINIMUM_SPIN_RATE_GYRO_CALIB 50.0 // degrees/second

void calibrate_gyros_obj(void) {
    fractional omegacorrPweighted[3];
    long calib_accum;
    int gain_change;
    unsigned int spin_rate_over2;
    if (mpuState.spin_rate > (unsigned int) (MINIMUM_SPIN_RATE_GYRO_CALIB * mpuState.DegPerSec)) {
        spin_rate_over2 = mpuState.spin_rate >> 1;
        VectorMultiply(3, omegacorrPweighted, mpuState.spin_axis, mpuState.omegacorrP); // includes 1/2

        calib_accum = __builtin_mulsu(omegacorrPweighted[0], (unsigned int) ((1.0 / HEARTBEAT_HZ) * mpuState.G_gain / GYRO_CALIB_TAU));
        gain_change = __builtin_divsd(calib_accum, spin_rate_over2);
        mpuState.ggain[0] = adjust_gyro_gain_obj(mpuState.ggain[0], gain_change);

        calib_accum = __builtin_mulsu(omegacorrPweighted[1], (unsigned int) ((1.0 / HEARTBEAT_HZ) * mpuState.G_gain / GYRO_CALIB_TAU));
        gain_change = __builtin_divsd(calib_accum, spin_rate_over2);
        mpuState.ggain[1] = adjust_gyro_gain_obj(mpuState.ggain[1], gain_change);

        calib_accum = __builtin_mulsu(omegacorrPweighted[2], (unsigned int) ((1.0 / HEARTBEAT_HZ) * mpuState.G_gain / GYRO_CALIB_TAU));
        gain_change = __builtin_divsd(calib_accum, spin_rate_over2);
        mpuState.ggain[2] = adjust_gyro_gain_obj(mpuState.ggain[2], gain_change);
    }
    return;
}

/*
void output_matrix(void)
//	This routine makes the direction cosine matrix evident
//	by setting the three servos to the three values in the
//	matrix.
{
        union longww accum ;
        accum.WW = __builtin_mulss( mpuState.rmat[6] , 4000 ) ;
//	PDC1 = 3000 + accum._.W1 ;
//	accum.WW = __builtin_mulss( mpuState.rmat[7] , 4000 ) ;
        accum.WW = __builtin_mulss( mpuState.rmat[3] , 4000 ) ;
        PDC2 = 3000 + accum._.W1 ;
        accum.WW = __builtin_mulss( mpuState.rmat[4] , 4000 ) ;
        PDC3 = 3000 + accum._.W1 ;
        return ;
}
 */

/*
void output_IMUvelocity(void)
{
        PDC1 = pulsesat( IMUvelocityx._.W1 + 3000 ) ;
        PDC2 = pulsesat( IMUvelocityy._.W1 + 3000 ) ;
        PDC3 = pulsesat( IMUvelocityz._.W1 + 3000 ) ;

//	PDC1 = pulsesat( mpuState.accelEarth[0] + 3000 ) ;
//	PDC2 = pulsesat( mpuState.accelEarth[1] + 3000 ) ;
//	PDC3 = pulsesat( mpuState.accelEarth[2] + 3000 ) ;

        return ;
}
 */

//extern void dead_reckon(void);
//extern void integrate_loc_cm(void);

void dcm_run_imu_step_obj(void)
//	update the matrix, renormalize it,
//	adjust for roll and pitch drift,
//	and send it to the servos.
{
    //TODO: fix deadReconing.c for dual IMUs
    //    integrate_loc_cm(); // experimental cm precision dead reckoning
    //    dead_reckon();
    /*	WJP - accel comp
    #if ( HILSIM != 1 )
            adj_accel() ;
    #endif
     */

    rupdate_obj();
    normalize_obj();
    roll_pitch_drift_obj();
#if (MAG_YAW_DRIFT == 1)
    if (magMessage == 7) {
        mag_drift();
    }
#endif
    PI_feedback_obj();
    calibrate_gyros_obj();
    return;
}

