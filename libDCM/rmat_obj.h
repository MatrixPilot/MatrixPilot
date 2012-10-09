/* 
 * File:   rmat_obj.h
 * Author: markw
 *
 * Created on August 29, 2012, 2:45 PM
 */

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

#ifndef RMAT_OBJ_H
#define	RMAT_OBJ_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "libDCM_internal.h"
#include "libDCM_defines.h"
#include "../libUDB/filters.h"

#define RMAX15 0b0110000000000000	//	1.5 in 2.14 format

#define MAX_ACCEL_SAMPLES (1.1 * HEARTBEAT_HZ)
#define ACCEL_SAMPLES_PER_SEC HEARTBEAT_HZ

    extern unsigned long uptime;
    extern struct ADchannel mpu_xaccel, mpu_yaccel, mpu_zaccel; // x, y, and z accelerometer channels
    extern struct ADchannel mpu_xrate, mpu_yrate, mpu_zrate; // x, y, and z gyro channels

    struct IMUstate {
        unsigned int KProllpitch;
        unsigned int KIrollpitch;
        unsigned int KPyaw;
        unsigned int KIyaw;
        float Scale_gyro;
        float Scale_accel;
        long Gravity;
        long long DegPerSec;    // one degree per second, in AtoD/2 units
        fractional G_gain;

        fractional ggain[3];

        unsigned int spin_rate;
        fractional spin_axis[3];

        unsigned int Gyro_sat;

        fractional rmat[9];

        fractional rmatDelayCompensated[9];
        unsigned long rmatDelayTime; // timestamp for rmatDelayCompensated

        // delta theta
        fractional theta[3];

        //	gyro rotation vector:
        fractional omegagyro[3];
        fractional omega[3];

        //	gyro correction vectors:
        fractional omegacorrP[3];
        fractional omegacorrI[3];

        //  acceleration, as measured in GPS earth coordinate system
        fractional accelEarth[3];

        //union longww accelEarthFiltered[3];

        long int accelerometer_earth_integral[3];
        int GPS_velocity_previous[3];
        unsigned int accelerometer_samples;

        int accelerometer_earth[3];
        int GPS_acceleration[3];
        int accelerometer_reference[3];
        int errorRP_earth[3];

        //	correction vector integrators ;
        union longww gyroCorrectionIntegral[3];

        //	accumulator for computing adjusted omega:
        fractional omegaAccum[3];

        //	acceleration, as measured in plane coordinate system
        fractional gplane[3];

        //	horizontal velocity over ground, as measured by GPS (Vz = 0 )
        fractional dirovergndHGPS[3];

        //	horizontal direction over ground, as indicated by Rmatrix
        fractional dirovergndHRmat[3];

        //	rotation angle equal to omega times integration factor:
        //  fractional theta[3];

        //	matrix buffer:
        //  fractional rbuff[9];

        //	vector buffer
        fractional errorRP[3];
        fractional errorYawground[3];
        fractional errorYawplane[3];

        //	measure of error in orthogonality, used for debugging purposes:
        fractional error;

#if(MAG_YAW_DRIFT == 1)
        fractional declinationVector[2];
#endif

#if(DECLINATIONANGLE_VARIABLE == 1)
        union intbb dcm_declination_angle;
#endif

        // boxcar integrator buffer
        struct boxCarState filterState;
        int boxCarBuff[ACC_BOX_N * ACC_BOX_LEN];
        long boxCarSum[ACC_BOX_N];
        int gplaneFilt[ACC_BOX_N];
    };

    extern struct IMUstate mpuState;
    void dcm_init_rmat_obj(void);
    void dcm_run_imu_step_obj(void);
    void read_accel_obj();
    void read_gyros_obj();

#ifdef	__cplusplus
}
#endif

#endif	/* RMAT_OBJ_H */

