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
#include "mpu6000.h"

char debug_buffer[256];
int db_index = 0;
void send_debug_line(void);
extern unsigned int mpu_data[7];
extern struct ADchannel mpu_temp;
extern int mpuCnt;
extern bool mpuDAV;

// dummy globals
boolean sendGPS;
int tailFlash;

int main(void)
{
    // Set up the libraries
    udb_init();
    dcm_init();
    MPU6000_init16();

    udb_serial_set_rate(115200);
    //	udb_serial_set_rate(230400) ;

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
        // No longer calibrating: solid RED and send debug output
        LED_RED = LED_ON;
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

        accum.WW = __builtin_mulss(rmat[6], 4000);
        udb_pwOut[ROLL_OUTPUT_CHANNEL] = udb_servo_pulsesat(3000 + accum._.W1);

        accum.WW = __builtin_mulss(rmat[7], 4000);
        udb_pwOut[PITCH_OUTPUT_CHANNEL] = udb_servo_pulsesat(3000 + accum._.W1);

        accum.WW = __builtin_mulss(rmat[4], 4000);
        udb_pwOut[YAW_OUTPUT_CHANNEL] = udb_servo_pulsesat(3000 + accum._.W1);
    }

    // Serial output at xHz
    if (telCnt++ >= (HEARTBEAT_HZ / 200))
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
                           mpuCnt, mpu_data[0], mpu_data[1], mpu_data[2], mpu_data[4], mpu_data[5], mpu_data[6], mpu_data[3]);
//                mpuCnt, XACCEL_VALUE, YACCEL_VALUE, ZACCEL_VALUE,
//                XRATE_VALUE, YRATE_VALUE, ZRATE_VALUE, mpu_temp.value);
#elif 1
    sprintf(debug_buffer, "%06i %06i %06i %06i %06i %06i %06i %06i %06i %06i\r\n",
            mpuCnt, 
            rmat[0], rmat[1], rmat[2],
            rmat[3], rmat[4], rmat[5],
            rmat[6], rmat[7], rmat[8]);
#elif 0
    sprintf(debug_buffer, "lat: %li, long: %li, alt: %li\r\nrmat: %06i, %06i, %06i, %06i, %06i, %06i, %06i, %06i, %06i\r\naccel: %06i, %06i, %06i\r\ngyro: %06i, %06i, %06i\r\n",
            lat_gps.WW, long_gps.WW, alt_sl_gps.WW,
            rmat[0], rmat[1], rmat[2],
            rmat[3], rmat[4], rmat[5],
            rmat[6], rmat[7], rmat[8],
            XACCEL_VALUE, YACCEL_VALUE, ZACCEL_VALUE,
            XRATE_VALUE, YRATE_VALUE, ZRATE_VALUE);
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
