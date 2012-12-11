// This file is part of the MatrixPilot RollPitchYaw demo.
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


// main program for testing the IMU.

// Used for serial debug output
#include <stdio.h>

#include "libDCM.h"
#include "spiUtils.h"

char debug_buffer[256];
int db_index = 0;
void send_debug_line(void);

// dummy globals
boolean sendGPS;
int tailFlash;

int main(void)
{
    // Set up the libraries
    udb_init();
    dcm_init();

    //    udb_serial_set_rate(115200);
    //	udb_serial_set_rate(230400) ;
    udb_serial_set_rate(57600);

    LED_GREEN = LED_OFF;

    // Start it up!
    udb_run(); // This never returns.

    return 0;
}


// Called every 1/2 second at high priority

void udb_background_callback_periodic(void)
{
    if (!dcm_flags._.calib_finished)
    {
        // If still calibrating, blink RED
        udb_led_toggle(LED_RED);
    }
    else
    {
        // No longer calibrating
        LED_RED = LED_OFF;
    }

    return;
}


// Called every time we get gps data (1, 2, or 4 Hz, depending on GPS config)

void dcm_callback_gps_location_updated(void)
{
    // Blink GREEN led to show that the GPS is communicating
    udb_led_toggle(LED_GREEN);
    return;
}


// Called at 40 Hz, before sending servo pulses

void dcm_servo_callback_prepare_outputs(void)
{
    static int telCnt = 0;
    if (!dcm_flags._.calib_finished)
    {
        udb_pwOut[ROLL_OUTPUT_CHANNEL] = 3000;
        udb_pwOut[PITCH_OUTPUT_CHANNEL] = 3000;
        udb_pwOut[YAW_OUTPUT_CHANNEL] = 3000;
    }
    else
    {
        union longww accum;
#if DUAL_IMU == 0
        accum.WW = __builtin_mulss(rmat[6], 4000);
        udb_pwOut[ROLL_OUTPUT_CHANNEL] = udb_servo_pulsesat(3000 + accum._.W1);

        accum.WW = __builtin_mulss(rmat[7], 4000);
        udb_pwOut[PITCH_OUTPUT_CHANNEL] = udb_servo_pulsesat(3000 + accum._.W1);

        accum.WW = __builtin_mulss(rmat[4], 4000);
        udb_pwOut[YAW_OUTPUT_CHANNEL] = udb_servo_pulsesat(3000 + accum._.W1);
#else
        accum.WW = __builtin_mulss(mpuState.rmat[6], 4000);
        udb_pwOut[ROLL_OUTPUT_CHANNEL] = udb_servo_pulsesat(3000 + accum._.W1);

        accum.WW = __builtin_mulss(mpuState.rmat[7], 4000);
        udb_pwOut[PITCH_OUTPUT_CHANNEL] = udb_servo_pulsesat(3000 + accum._.W1);

        accum.WW = __builtin_mulss(mpuState.rmat[4], 4000);
        udb_pwOut[YAW_OUTPUT_CHANNEL] = udb_servo_pulsesat(3000 + accum._.W1);
#endif
    }

    // Serial output at xHz
    if (++telCnt >= (HEARTBEAT_HZ / 10))
    {
        telCnt = 0;
        send_debug_line();
    }

    return;
}


// Prepare a line of serial output and start it sending

void send_debug_line(void)
{
    db_index = 0;
#if 0
    sprintf(debug_buffer, "%06u axyz %06i %06i %06i gxyz %06i %06i %06i t %i\r\n",
            //                           mpuCnt, mpu_data[0], mpu_data[1], mpu_data[2], mpu_data[4], mpu_data[5], mpu_data[6], mpu_data[3]);
            mpuCnt, XACCEL_VALUE, YACCEL_VALUE, ZACCEL_VALUE,
            XRATE_VALUE, YRATE_VALUE, ZRATE_VALUE, mpu_temp.value);
#elif 0
    sprintf(debug_buffer, "%06i %06i %06i %06i %06i %06i %06i %06i %06i %06i\r\n",
            mpuCnt,
            rmat[0], rmat[1], rmat[2],
            rmat[3], rmat[4], rmat[5],
            rmat[6], rmat[7], rmat[8]);
#elif 0
    // this code is copied from MAVLink.c
    // It generates roll angles with positive representing right bank
    // i.e. CCW rotation looking down the longitudinal (x) axis.
    // It generates pitch angles with positive representing pitch up,
    // i.e. CCW rotation looking down the y axis (right wing).
    // CCW Yaw rotation is positive looking down Z axis (up at board).
#define 	BYTE_CIR_16_TO_RAD  ((2.0 * 3.14159265) / 65536.0 ) // Conveert 16 bit byte circular to radians
    struct relative2D matrix_accum;
    int accum;
    float roll, pitch, yaw;

    //  Roll: Earth Frame of Reference
    matrix_accum.x = rmat[8];
    matrix_accum.y = rmat[6];
    accum = rect_to_polar16(&matrix_accum); // binary angle (0 to 65536 = 360 degrees)
    roll = (180 / PI) * (-accum) * BYTE_CIR_16_TO_RAD; // Convert to degrees

    //  Pitch: Earth Frame of Reference
    //  Note that we are using the matrix_accum.x
    //  left over from previous rect_to_polar in this calculation.
    //  so this Pitch calculation must follow the Roll calculation
    matrix_accum.y = rmat[7];
    accum = -rect_to_polar16(&matrix_accum); // binary angle (0 to 65536 = 360 degrees)
    pitch = (180 / PI) * (accum) * BYTE_CIR_16_TO_RAD; // Convert to degrees

    // Yaw: Earth Frame of Reference
    matrix_accum.x = rmat[4];
    matrix_accum.y = rmat[1];
    accum = rect_to_polar16(&matrix_accum); // binary angle (0 to 65536 = 360 degrees)
    yaw = (180 / PI) * (-accum * BYTE_CIR_16_TO_RAD); // Convert to degrees

    sprintf(debug_buffer, "AAA,RS:%1.0f,PS:%1.0f,AS:%1.0f,LRI:%1.0f,H:%i,AGL:%i\r\nCCC,ALH:%i,CRS:%i,BER:%i,WPN:%i,DST:%i\r\n",
            100 * roll, 100 * pitch, roll, 100 * pitch, 0, 0, //(17.5+roll*35.0/180), 100*(240+(pitch*40/90)), 0, 0,
            0, (int) yaw, 0, 0, 0);
#elif DUAL_IMU == 0
    sprintf(debug_buffer, "lat: %li, long: %li, alt: %li\r\nrmat: %06i, %06i, %06i, %06i, %06i, %06i, %06i, %06i, %06i\r\naccel: %06i, %06i, %06i\r\ngyro: %06i, %06i, %06i\r\n",
            lat_gps.WW, long_gps.WW, alt_sl_gps.WW,
            rmat[0], rmat[1], rmat[2],
            rmat[3], rmat[4], rmat[5],
            rmat[6], rmat[7], rmat[8],
            XACCEL_VALUE, YACCEL_VALUE, ZACCEL_VALUE,
            XRATE_VALUE, YRATE_VALUE, ZRATE_VALUE);
#elif DUAL_IMU == 1
    sprintf(debug_buffer, "lat: %li, long: %li, alt: %li\r\nrmat: %06i, %06i, %06i, %06i, %06i, %06i, %06i, %06i, %06i\r\naccel: %06i, %06i, %06i\r\ngyro: %06i, %06i, %06i\r\n",
            lat_gps.WW, long_gps.WW, alt_sl_gps.WW,
            mpuState.rmat[0], mpuState.rmat[1], mpuState.rmat[2],
            mpuState.rmat[3], mpuState.rmat[4], mpuState.rmat[5],
            mpuState.rmat[6], mpuState.rmat[7], mpuState.rmat[8],
            XACCEL_MPU, YACCEL_MPU, ZACCEL_MPU,
            XRATE_MPU, YRATE_MPU, ZRATE_MPU);
#endif

    udb_serial_start_sending_data();

    return;
}


// Return one character at a time, as requested.
// Requests will stop after we send back a -1 end-of-data marker.

int udb_serial_callback_get_byte_to_send(void)
{
    unsigned char c = debug_buffer[ db_index++ ];

    if (c == 0) return -1;

    return c;
}


// Don't respond to serial input

void udb_serial_callback_received_byte(char rxchar)
{
    // Do nothing
    return;
}

void udb_callback_radio_did_turn_off(void)
{
}

void run_background_task(void)
{
    // wait for interrupt to save a little power
    // adds 2 cycles of interrupt latency (125 nsec at 16MHz, 50ns at 40MHz)
    Idle();

    return;
}
