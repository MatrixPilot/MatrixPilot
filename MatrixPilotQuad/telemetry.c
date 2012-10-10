// This file is part of the MatrixPilotQuad firmware.
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

#include "../libDCM/libDCM.h"
#include "../libDCM/rmat_obj.h"

// Used for serial debug output
#include <stdio.h>
#include <string.h>
#include "../libUDB/libUDB_internal.h"
#include "../libDCM/libDCM_internal.h"

//int db_index = 0;
boolean hasWrittenHeader = 0;
int header_line = 0;
boolean sendGains = false;
boolean sendGPS = false;

extern unsigned int mpu_data[7], mpuCnt;

extern fractional gplaneFilt[3];
extern fractional omegacorrP[];
extern fractional omegacorrI[];
extern fractional errorRP[];
extern fractional accelerometer_earth[];
extern fractional magFieldEarth[3];
extern fractional velErrorEarth[3];
extern fractional udb_magFieldBody[3];
extern fractional udb_magOffset[3];
extern fractional magAlignment[4];
extern unsigned long rmatDelayTime;
extern unsigned int desired_heading, earth_yaw;
extern int theta[3], roll_control, pitch_control, yaw_control, accelEarth[3], accel_feedback;
extern fractional velocityErrorEarth[3];
extern fractional locationErrorEarth[3];
extern struct relative3D GPSloc_cm;
extern struct relative3D GPSvelocity;

extern int flight_mode, pos_error[], pos_perr[], pos_derr[];

extern int commanded_roll, commanded_pitch, commanded_yaw, pwManual[];
extern int poscmd_north, poscmd_east;
extern int roll_error, pitch_error, yaw_error, yaw_rate_error;
extern int rate_error[3], rate_error_dot[2];
extern int rolladvanced, pitchadvanced;
extern signed char lagBC, precessBC;

extern union longww IMUcmx, IMUcmy, IMUcmz;
extern union longww IMUvx, IMUvy, IMUvz;
extern union longww roll_error_integral, pitch_error_integral, yaw_error_integral;
extern unsigned int pid_gains[4];

// 10,000 counts is 100%
extern unsigned int cpu_timer;

extern struct ADchannel udb_vref;
extern union longww primary_voltage; // primary battery voltage

extern unsigned long uptime;

extern unsigned int throttle_limit;

volatile int trap_flags __attribute__((persistent));
volatile long trap_source __attribute__((persistent));
volatile int osc_fail_count __attribute__((persistent));

#define DEBUGLEN 256
char debug_buffer[DEBUGLEN];
extern boolean pauseSerial;

// assuming OpenLog needs a .25 second buffer and baud rate is 2*115.2K
// we need 2*.25 * 11.52K = 5760 bytes, ~5K more than OpenLog V3 light's 800 bytes.

// This version of telemetry requires OpenLog_v3_Light and modified core files
// SerialPort.h and SerialPort.cpp

// ring buffer code ported from Arduino SerialPort Library (C) 2011 GPLV3 by William Greiman.
// there was a serious bug in put(char*, int), fixed here in ring_putn(char*, int).
// RINGLEN is the usable number of bytes, RINGSIZE is the actual sizeof(ring_buffer).
// ring_tail is modified by ring_get at IPL5 when transmitting data via UART2.
// ring_head is not modified by ISRs: Since this is a transmit buffer, data is added
// to the queue by calling one of the put methods at IPL0.
#if BOARD_TYPE == UDB4_BOARD
#define RINGLEN 5800
#elif BOARD_TYPE & AUAV2_BOARD
#define RINGLEN 5800
extern unsigned int sFrameLost;
unsigned int lastFrameLost = -1;
#else
#error("unsupported board type")
#endif

#define RINGSIZE (RINGLEN+1)
static volatile int ring_head = 0;
static volatile int ring_tail = 0;
__attribute__((far)) char ring_buffer[RINGSIZE];

// called by udb_serial_callback_get_byte_to_send at IPL5
// modifies ring_tail

boolean ring_get(char* b) {
    int t = ring_tail;
    if (ring_head == t) {
        // buffer is empty
        return false;
    }
    *b = ring_buffer[t++];
    ring_tail = t < RINGSIZE ? t : 0;
    return true;
}

// insert 1 byte at head of buffer
// return number of bytes stored, zero if buffer is full

int ring_put(char b) {
    int h = ring_head;
    // OK to store here even if ring is full
    ring_buffer[h++] = b;
    if (h >= RINGSIZE) h = 0;
    if (h == ring_tail) return 0; // buffer is full, didn't lose b (yet)
    ring_head = h;
    return 1;
}

// insert n bytes at head of buffer; modifies head at IPL5
// If space available is less than n, store only space bytes.
// return number of bytes stored, zero if buffer is full

int ring_putn(const char* b, int n) {
    // raise interrupt priority to 5 to fetch ring_tail
    int ipl = SRbits.IPL;
    SRbits.IPL = 5;
    int t = ring_tail;
    // restore interrupt priority
    SRbits.IPL = ipl;
    int h = ring_head;
    int space;
    if (h < t) {
        space = t - h - 1;
    } else {
        space = RINGLEN - h + t;
    }
    if (n > space) n = space;
    if ((n + h) <= RINGSIZE) {
        memcpy(&ring_buffer[h], b, n);
    } else {
        int n1 = RINGSIZE - h;
        memcpy(&ring_buffer[h], b, n1);
        memcpy(ring_buffer, &b[n1], n - n1);
    }
    h += n;
    ring_head = h < RINGSIZE ? h : h - RINGSIZE;
    return n;
}

// return number of bytes in buffer

int ring_available() {
    // raise interrupt priority to 5 to access ring_tail
    int ipl = SRbits.IPL;
    SRbits.IPL = 5;

    int n = ring_head - ring_tail;

    // restore interrupt priority
    SRbits.IPL = ipl;

    return n < 0 ? RINGSIZE + n : n;
}

// return space available in buffer (in bytes)

int ring_space() {
    int space;
    // raise interrupt priority to 5 to access ring_tail
    int ipl = SRbits.IPL;
    SRbits.IPL = 5;
    int t = ring_tail;
    // restore interrupt priority
    SRbits.IPL = ipl;

    if (ring_head < t) {
        space = t - ring_head - 1;
    } else {
        space = RINGLEN - ring_head + t;
    }

    return space;
}

void queue_data(const char* buff, int nbytes) {
    if (ring_space() > nbytes) {
        ring_putn(buff, nbytes);
        udb_serial_start_sending_data();
    }
}

// send string out telemetry port

void queue_string(const char* string) {
    //    db_index = 0;
    queue_data(string, strlen(string));
}

// queue a string without null terminator

void queue_prepend(const char* buff, int nbytes) {
    if (ring_space() > nbytes - 1) {
        ring_putn(buff, nbytes - 1);
    }
}

// format gains string

static const char gainsHeader[] = "HEARTBEAT_HZ,  PID_HZ, TILT_KP, TILT_KI, ACRO_KP, RATE_KP, RATE_KI, RATE_KD,  YAW_KP,  YAW_KI,  YAW_KD, ACCEL_K\r\n";

int fmtGains(char* buff, int buffLen) {
    return snprintf(buff, buffLen, "%12i, %7i, %7.3f, %7.3f, %7.3f, %7.3f, %7.3f, %7.3f, %7.3f, %7.3f, %7.3f, %7.3f\r\n",
            HEARTBEAT_HZ, PID_HZ,
            (double) pid_gains[TILT_KP_INDEX] / RMAX,
            (double) pid_gains[TILT_KI_INDEX] / (256.0 * RMAX / ((double) PID_HZ)),
            (double) pid_gains[ACRO_KP_INDEX] / RMAX,
            (double) pid_gains[RATE_KP_INDEX] / RMAX,
            (double) pid_gains[RATE_KI_INDEX] / (256.0 * RMAX / ((double) PID_HZ)),
            (double) pid_gains[RATE_KD_INDEX] / RMAX,
            (double) pid_gains[YAW_KP_INDEX] / RMAX,
            (double) pid_gains[YAW_KI_INDEX] / (256.0 * RMAX / ((double) PID_HZ)),
            (double) pid_gains[YAW_KD_INDEX] / RMAX,
            (double) pid_gains[ACCEL_K_INDEX] / RMAX
            );
}

#if TELEMETRY_TYPE == 0
static const char tel_header[] = " tick,   r6,   r7,   yaw,   w0,   w1,   w2,  rfb,  pfb,  yfb, rerr,rerrI, perr,perrI, yerr,yerrI, rcmd, pcmd, ycmd,  thr,accfb,  cpu, vref, rpm3\r\n";
#elif TELEMETRY_TYPE == 1
static const char tel_header[] = " tick,  r0,   r1,   r2,   r3,   r4,   r5,   r6,   r7,    r8,  th0,  th1,  th2, as3d,estwx,estwy,estwz,imuvx,imuvy,imuvz,imulx,imuly,imulz\r\n";
#elif TELEMETRY_TYPE == 2
static const char tel_header[] = " tick,dtick,   r6,   r7,   r8, eyaw,  th0,  th1,  th2, cyaw, dhdg, eyaw,magEx,magEy,magEz,magAx,magAy,magAz,magAs,magBx,magBy,magBz,magOx,magOy,magOz\r\n";
#elif TELEMETRY_TYPE == 3
static const char tel_header[] = " tick,cmdYaw,desHdg,earthYaw,GPSloc_cm, magAlignment, magOffset, IMU_velocity, IMU_location\r\n";
#elif TELEMETRY_TYPE == 4
static const char tel_header[] = " tick,   r6,   r7,   yaw,   w0,   w1,   w2, rcmd, pcmd, ycmd, rerr,rerrI, perr,perrI, yerr,yerrI,erat0,erat1,erat2,edot0,edot1,  rfb,  pfb,  yfb,  thr,accfb,  cpu,   m3,  rpm3\r\n";
#elif TELEMETRY_TYPE == 5
static const char tel_header[] = " tick,   r6,   r7,   yaw,   w0,   w1,   w2, rcmd, pcmd, ycmd, rerr,rerrI, perr,perrI, yerr,yerrI,erat0,erat1,erat2,primV, mode,  rfb,  pfb,  yfb, accx, accy, accz,  thr,  cpu\r\n";
#elif TELEMETRY_TYPE == 6
static const char tel_header[] = " tick,   r6,   r7,   yaw,   w0,   w1,   w2, accx, accy, accz, mpu0, mpu1, mpu2, mpu3, mpu4, mpu5, mpu6,mpuct,  thr,  cpu\r\n";
#elif TELEMETRY_TYPE == 7
static const char tel_header[] = " tick,   r0,   r1,   r2,   r3,   r4,   r5,   r6,   r7,   r8,   w0,   w1,   w2, gplx, gply, gplz, erpx, erpy, erpz, ocpx, ocpy, ocpz, ocix, ociy, ociz\r\n";
#elif TELEMETRY_TYPE == 8
static const char tel_header[] = " tick,   r6,   r7,   yaw,   w0,   w1,   w2, accx, accy, accz,   r6,   r7,   yaw,   w0,   w1,   w2, accx, accy, accz,primV,  thr,  cpu\r\n";
#endif

// Prepare a line of serial output and start it sending

void send_telemetry(void) {
    //    db_index = 0;
    //    union longww IMUlocx, IMUlocy, IMUlocz;

    if (!hasWrittenHeader) {
        header_line++;
        switch (header_line) {
            case 1:
                snprintf(debug_buffer, sizeof (debug_buffer), "\r\n");
                queue_string(debug_buffer);
                break;
            case 2:
                queue_string(gainsHeader);
                break;
            case 3:
                fmtGains(debug_buffer, sizeof (debug_buffer));
                queue_string(debug_buffer);
                break;
            case 4:
                snprintf(debug_buffer, sizeof (debug_buffer),
                        "telemetry type: %i, baud: %li, freq: %i Hz\r\n",
                        TELEMETRY_TYPE, TELEMETRY_BAUD, TELEMETRY_HZ);
                queue_string(debug_buffer);
                break;
            case 5:
                queue_string((char*) tel_header);
                hasWrittenHeader = 1;
                break;
            default:
                hasWrittenHeader = 1;
                break;
        }
    } else {
#if TELEMETRY_TYPE == 0
        // standard
        // scale input channel 7 period to RPM
        // from units of 0.5usec: RPM = 60sec * 1 / period sec
        int rpm;
        if (udb_pwIn[7] > 0) {

            float freq = 2.0E6 / udb_pwIn[7];
            rpm = (int) (freq * COMFREQ_TO_RPM);
        }
        // 146 characters per record: 11,520/146 = 78Hz; 22,220/146 = 152Hz
        snprintf(debug_buffer, sizeof (debug_buffer), "%5li,%5i,%5i,%6i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%6i\r\n",
                uptime,
                rmat[6], rmat[7], earth_yaw,
                theta[0], theta[1], theta[2],
                roll_control, pitch_control, yaw_control,
                //                              rolladvanced, pitchadvanced,
                //                              omegagyro[2], lagBC,
                roll_error, roll_error_integral._.W1,
                pitch_error, pitch_error_integral._.W1,
                yaw_error, yaw_error_integral._.W1,
                commanded_roll, commanded_pitch, commanded_yaw, pwManual[THROTTLE_INPUT_CHANNEL],
                accel_feedback, cpu_timer, udb_vref.value, rpm);
        //                              accel_feedback, cpu_timer, udb_vref.value, precessBC);
#elif TELEMETRY_TYPE == 1
        // IMU log: 23 fields
        // 140 characters per record
        snprintf(debug_buffer, sizeof (debug_buffer), "%5li,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i\r\n",
                uptime,
                rmat[0], rmat[1], rmat[2],
                rmat[3], rmat[4], rmat[5],
                rmat[6], rmat[7], rmat[8],
                theta[0], theta[1], theta[2],
                air_speed_3DIMU,
                estimatedWind[0], estimatedWind[1], estimatedWind[2],
                IMUvelocityx._.W1, IMUvelocityy._.W1, IMUvelocityz._.W1,
                IMUlocationx._.W1, IMUlocationy._.W1, IMUlocationz._.W1
                );
#elif TELEMETRY_TYPE == 2
        // IMU/mag log: 25 fields
        // parser: parseLogMag.py analyzer: procLogMag
        // 145 characters per record
        snprintf(debug_buffer, sizeof (debug_buffer), "%5li,%5li,%5i,%5i,%5i,%5u,%5i,%5i,%5i,%5i,%5u,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i\r\n",
                uptime, rmatDelayTime,
                rmat[6], rmat[7], rmat[8],
                earth_yaw,
                theta[0], theta[1], theta[2],
                commanded_yaw, desired_heading, yaw_error,
                magFieldEarth[0], magFieldEarth[1], magFieldEarth[2],
                magAlignment[0], magAlignment[1], magAlignment[2], magAlignment[3],
                udb_magFieldBody[0], udb_magFieldBody[1], udb_magFieldBody[2],
                udb_magOffset[0], udb_magOffset[1], udb_magOffset[2]
                );
#elif TELEMETRY_TYPE == 3
        // deadReckoning log: 23 fields (parseLogLoc.py)
        // 125 characters per record
        snprintf(debug_buffer, sizeof (debug_buffer), "%5li,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i\r\n",
                uptime,
                commanded_yaw, desired_heading, earth_yaw,
                GPSloc_cm.x, GPSloc_cm.y, GPSloc_cm.z,
                poscmd_east, poscmd_north, pos_perr[1] - pos_derr[1],
                pos_error[0], pos_error[1], flight_mode,
                pos_perr[0], pos_derr[0], pos_perr[1], pos_derr[1],
                IMUvx._.W1, IMUvy._.W1, IMUvz._.W1,
                IMUcmx._.W1, IMUcmy._.W1, IMUcmz._.W1
                );
#elif TELEMETRY_TYPE == 4
        // PID controller log: 29 fields
        // 147 characters per record: 222,222/1370 = 150Hz
        // scale input channel 7 period to RPM
        // from units of 0.5usec: RPM = 60sec * 1 / period sec
        int rpm;
        if (udb_pwIn[7] > 0) {

            float freq = 2.0E6 / udb_pwIn[7];
            rpm = (int) (freq * COMFREQ_TO_RPM);
        }
        snprintf(debug_buffer, sizeof (debug_buffer), "%5li,%5i,%5i,%6i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i\r\n",
                uptime,
                rmat[6], rmat[7], earth_yaw,
                omegagyro[0], omegagyro[1], omegagyro[2],
                commanded_roll, commanded_pitch, commanded_yaw,
                roll_error, roll_error_integral._.W1,
                pitch_error, pitch_error_integral._.W1,
                yaw_error, yaw_error_integral._.W1,
                rate_error[0], rate_error[1], rate_error[2],
                rate_error_dot[0], rate_error_dot[1],
                roll_control, pitch_control, yaw_control,
                pwManual[THROTTLE_INPUT_CHANNEL],
                accel_feedback, cpu_timer, udb_pwOut[3], rpm);
#elif TELEMETRY_TYPE == 5
#if DUALIMU == 1
        // pointer to rotation matrix
        fractional* prmat = &(mpuState.rmat[0]);
        // pointer to omegagyro vector
        fractional* pomegagyro = &(mpuState).omegagyro[0];
#else
        fractional* prmat = &rmat[0];
        fractional* pomegagyro = &omegagyro[0];
#endif

        // PID controller log2: 29 fields
        // 147 characters per record: 222,222/1470 = 150Hz
        snprintf(debug_buffer, sizeof (debug_buffer), "%5li,%5i,%5i,%6i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i\r\n",
                uptime,
                prmat[6], prmat[7], earth_yaw,
                pomegagyro[0], pomegagyro[1], pomegagyro[2],
                commanded_roll, commanded_pitch, commanded_yaw,
                roll_error, roll_error_integral._.W1,
                pitch_error, pitch_error_integral._.W1,
                yaw_error, yaw_error_integral._.W1,
                rate_error[0], rate_error[1], rate_error[2],
                primary_voltage._.W1, flight_mode,
                roll_control, pitch_control, yaw_control,
                gplaneFilt[0], gplaneFilt[1], gplaneFilt[2],
                pwManual[THROTTLE_INPUT_CHANNEL], cpu_timer);
#elif TELEMETRY_TYPE == 6
        // MPU6000 test: 20 fields
        // parser: parseLog6000.py
        // ~130 characters per record: 222,222/1300 = 170Hz
        snprintf(debug_buffer, sizeof (debug_buffer), "%5li,%5i,%5i,%6i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i\r\n",
                uptime,
                rmat[6], rmat[7], earth_yaw,
                omegagyro[0], omegagyro[1], omegagyro[2],
                gplaneFilt[0], gplaneFilt[1], gplaneFilt[2],
                mpu_data[0], mpu_data[1], mpu_data[2],
                mpu_data[3],
                mpu_data[4], mpu_data[5], mpu_data[6],
                mpuCnt,
                pwManual[THROTTLE_INPUT_CHANNEL], cpu_timer);
#elif TELEMETRY_TYPE == 7
        // IMU log: 25 fields
        // 152 characters per record
        snprintf(debug_buffer, sizeof (debug_buffer), "%5li,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i\r\n",
                uptime,
                rmat[0], rmat[1], rmat[2],
                rmat[3], rmat[4], rmat[5],
                rmat[6], rmat[7], rmat[8],
                omegagyro[0], omegagyro[1], omegagyro[2],
                gplaneFilt[0], gplaneFilt[1], gplaneFilt[2],
                errorRP[0], errorRP[1], errorRP[2],
                omegacorrP[0], omegacorrP[1], omegacorrP[2],
                omegacorrI[0], omegacorrI[1], omegacorrI[2]
                );
#elif TELEMETRY_TYPE == 8
        // dual IMU test: 21 fields
        // parser: parseLog_type8.py
        // ~130 characters per record: 222,222/1300 = 170Hz
        struct relative2D matrix_accum;
        matrix_accum.x = mpuState.rmat[4];
        matrix_accum.y = mpuState.rmat[1];
        unsigned int mpu_yaw; // yaw with respect to earth frame
        mpu_yaw = rect_to_polar16(&matrix_accum); // binary angle (0 : 65536 = 360 degrees)
        snprintf(debug_buffer, sizeof (debug_buffer), "%5li,%5i,%5i,%6i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%6i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i,%5i\r\n",
                uptime,
                rmat[6], rmat[7], earth_yaw,
                omegagyro[0], omegagyro[1], omegagyro[2],
                gplaneFilt[0], gplaneFilt[1], gplaneFilt[2],
                mpuState.rmat[6], mpuState.rmat[7], mpu_yaw,
                mpuState.omegagyro[0], mpuState.omegagyro[1], mpuState.omegagyro[2],
                mpuState.gplaneFilt[0], mpuState.gplaneFilt[1], mpuState.gplaneFilt[2],
                primary_voltage._.W1, pwManual[THROTTLE_INPUT_CHANNEL], cpu_timer);
#endif
        queue_string(debug_buffer);

#if BOARD_TYPE == AUAV2_BOARD_ALPHA1
        if (sFrameLost != lastFrameLost) {
            lastFrameLost = sFrameLost;
            snprintf(debug_buffer, sizeof (debug_buffer), "S.bus frames lost: %i\r\n", sFrameLost);
            queue_string(debug_buffer);
        }
#endif
        // check for low battery voltage
        if (throttle_limit < (unsigned int) (THROTTLE_LIMIT * 65536)) {
            snprintf(debug_buffer, sizeof (debug_buffer), "throttle_limit: %f\r\n",
                (double) throttle_limit / 65536.0);
            queue_string(debug_buffer);
        }

        if (sendGPS) {
            sendGPS = false;
            // record format: gps: N, E, A,
            snprintf(debug_buffer, sizeof (debug_buffer), "gps: %li,%li,%li,%li,%li,%u,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i\r\n",
                    uptime, tow.WW, lat_gps.WW, long_gps.WW, alt_sl_gps.WW,
                    (unsigned int) cog_gps.BB, sog_gps.BB,
                    air_speed_3DIMU,
                    estimatedWind[0], estimatedWind[1], estimatedWind[2],
                    IMUvelocityx._.W1, IMUvelocityy._.W1, IMUvelocityz._.W1,
                    IMUlocationx._.W1, IMUlocationy._.W1, IMUlocationz._.W1
                    );
            queue_string(debug_buffer);
        }
        if (sendGains) {
            sendGains = false;
            queue_prepend((char*) gainsHeader, sizeof (gainsHeader));
            fmtGains(debug_buffer, sizeof (debug_buffer));
            queue_string(debug_buffer);
            //            queue_data((char*) tel_header, strlen(tel_header));
        }
    }

    return;
}

/*
extern int gplane[] ;
// Prepare a line of serial output and start it sending
void send_debug_line( void )
{
db_index = 0 ;

if (!hasWrittenHeader)
{
sprintf(debug_buffer, "w0,w1,w2,a0,a1,a2\r\n") ;
hasWrittenHeader = 1 ;
}
else
{
sprintf(debug_buffer, "%i,%i,%i,%i,%i,%i\r\n" ,
omegagyro[0],	omegagyro[1],omegagyro[2] ,
gplane[0],gplane[1],gplane[2]
) ;
}

udb_serial_start_sending_data() ;

return ;
}
 */

// Return one character at a time, as requested.
// Requests will stop after we send back a -1 end-of-data marker.
// called by _U2TXInterrupt at IPL5

int udb_serial_callback_get_byte_to_send(void) {
    char c = -1;
    boolean status = false;

    if (!pauseSerial)
        status = ring_get(&c);

    if (!status || (c == 0)) c = -1;

    return c;
}


// Control characters for serial input software flow control
#define XOFF 19
#define XON 17

void udb_serial_callback_received_byte(char rxchar) {
    // check for XON/XOFF

    if (rxchar == XON) {
        if (pauseSerial) {
            pauseSerial = false;
            udb_serial_start_sending_data();
        }
    } else if (rxchar == XOFF) {
        pauseSerial = true;
    }

    //    // check for XON/XOFF
    //    if (rxchar == XOFF)
    //    {
    //        pauseSerial = true;
    //    }
    //    else if (rxchar == XON)
    //    {
    //        if (pauseSerial)
    //        {
    //            pauseSerial = false;
    //            udb_serial_start_sending_data();
    //        }
    //    }
    return;
}


