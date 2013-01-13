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

//#include <stdio.h>

#include "../../libDCM/libDCM.h"
#include "options.h"
#include "../../libUDB/spiUtils.h"
#include "../../libUDB/mpu6000.h"

boolean didCalibrate = false;

void send_fast_telemetry(void);
void send_telemetry(void);
void motorCntrl(void);
void setup_origin(void);
void storeGains(void);

extern char debug_buffer[256];
void log_string(char*);

extern unsigned int pid_gains[];
extern unsigned long uptime;
extern boolean sendGains;
extern boolean sendGPS;

// decoded flight mode switch
int flight_mode = 0;

// decoded gain mode switch
int gainadj_mode = 0;

extern int pitch_step;

// decoded failsafe mux input: true means UDB outputs routed to motors, false means RX throttle to motors
boolean udb_throttle_enable = false;

boolean writeGains = false;

unsigned int tailFlash = 0;

extern union longww primary_voltage;
extern unsigned int lowVoltageWarning;

int main(void)
{
    // Set up the libraries
    udb_init();
    dcm_init();

//    // Initialize MPU-6000 to report filtered sensor values at 200Hz
//    MPU6000_init16();

    //#warning("GPS yaw drift correction disabled")
    //    dcm_enable_yaw_drift_correction(false);

#if (ENABLE_GAINADJ != 0)
    // read saved gains
    eeprom_SequentialRead(PID_GAINS_BASE_ADDR, (unsigned char *) pid_gains, 2 * PID_GAINS_N);
#else
    // init gains
    pid_gains[TILT_KP_INDEX] = (unsigned int) (RMAX * TILT_KP);
    pid_gains[RATE_KP_INDEX] = (unsigned int) (RMAX * RATE_KP);
    pid_gains[RATE_KD_INDEX] = (unsigned int) (RMAX * RATE_KD);
    pid_gains[YAW_KP_INDEX] = (unsigned int) (RMAX * YAW_KP);
    pid_gains[YAW_KD_INDEX] = (unsigned int) (RMAX * YAW_KD);
    pid_gains[ACCEL_K_INDEX] = (unsigned int) (RMAX * ACCEL_K);
    pid_gains[ACRO_KP_INDEX] = (unsigned int) (RMAX * ACRO_KP);

    pid_gains[TILT_KI_INDEX] = (unsigned int) (256.0 * RMAX * TILT_KI / ((double) PID_HZ));
    pid_gains[YAW_KI_INDEX] = (unsigned int) (256.0 * RMAX * YAW_KI / ((double) PID_HZ));
    pid_gains[RATE_KI_INDEX] = (unsigned int) (256.0 * RMAX * RATE_KI / ((double) PID_HZ));

    // set flag to save gains in eeprom
    writeGains = 1;
#endif

    // OpenLog's (calculated) actual baud rate is 222,222 when set up for 230,400
    // minicom set at 230,400 baud works fine with OpenLog at 230,400
    udb_serial_set_rate(TELEMETRY_BAUD); // this works with OpenLog set at 230,400 baud

    LED_GREEN = LED_OFF;
    TAIL_LIGHT = LED_OFF; // taillight off

    // Start it up!
    udb_run(); // This never returns.

    return 0;
}

void check_failsafe()
{
    // decode flight mode input
    if (udb_pwIn[FAILSAFE_MUX_CHANNEL] < FAILSAFE_MUX_THRESH)
        udb_throttle_enable = true;
    else
        udb_throttle_enable = false;
}

void check_flight_mode(void)
{
    // decode flight mode input
    if (udb_pwIn[FLIGHT_MODE_CHANNEL] < FLIGHT_MODE_THRESH1)
    {
        flight_mode = FLIGHT_MODE_2;
        gainadj_mode = 2;
    }
    else if (udb_pwIn[FLIGHT_MODE_CHANNEL] < FLIGHT_MODE_THRESH2)
    {
        flight_mode = FLIGHT_MODE_1;
        gainadj_mode = 1;
    }
    else
    {
        flight_mode = FLIGHT_MODE_0;
        gainadj_mode = 0;
    }
}

void storeGain(int index)
{
    if (index >= 0 && index < PID_GAINS_N)
    {
        // save to EEPROM
        unsigned int address = PID_GAINS_BASE_ADDR + (2 * index);
        eeprom_ByteWrite(address++, (unsigned char) pid_gains[index]);
        eeprom_ByteWrite(address, (unsigned char) (pid_gains[index] >> 8));
    }
}

void storeGains(void)
{
    int index;
    for (index = 0; index < PID_GAINS_N; index++)
    {
        // save to EEPROM
        unsigned int address = PID_GAINS_BASE_ADDR + (2 * index);
        eeprom_ByteWrite(address++, (unsigned char) pid_gains[index]);
        eeprom_ByteWrite(address, (unsigned char) (pid_gains[index] >> 8));
    }
    //    eeprom_PageWrite(PID_GAINS_BASE_ADDR, (unsigned char*) pid_gains, 2 * PID_GAINS_N);
}

void adjust_gain(int index, int delta)
{
    if (delta > 0)
    {
        delta = RMAX * GAIN_INC;
        if (pid_gains[index] < (0xFFFF - delta))
        {
            tailFlash = 1;
            pid_gains[index] += delta;
        }
        else
        {
            tailFlash = 5;
            pid_gains[index] = 0xFFFF;
        }

    }
    else
    {
        delta = RMAX * GAIN_INC;
        if (pid_gains[index] > delta)
        {
            tailFlash = 2;
            pid_gains[index] -= delta;
        }
        else
        {
            tailFlash = 5;
            pid_gains[index] = 0;
        }
    }
}

// map flight modes [0,1,2] to gain indices
int gainAdjIndex[] = {ADJ_GAIN_0, ADJ_GAIN_1, ADJ_GAIN_2};

static boolean throttleUp = false;

void check_gain_adjust(void)
{
    static int gainState = -1; // gainState = {-1:init, 0:mode select, 1:adjust gain}
    static int gainIndex = 0;
    static int lastGainChVal = 0;
    // To change a gain, set flight mode switch to desired gain adjust index.
    // gain is adjusted, whenever GAIN_CHANNEL changes by more than 6 PWM counts.
    switch (gainState)
    {
    case -1: // init
        if (dcm_flags._.calib_finished)
        {
            // initialize lastGainChVal after cal complete
            lastGainChVal = udb_pwIn[GAIN_CHANNEL];
            gainState = 0;
        }
        break;
    case 0: // adjust gain
        gainIndex = gainAdjIndex[gainadj_mode];
        // increment or decrement gain value by GAIN_INC, based on change in GAIN_CHANNEL
        int gain_delta = udb_pwIn[GAIN_CHANNEL] - lastGainChVal;
        if (abs(gain_delta) > 6)
        {
            lastGainChVal = udb_pwIn[GAIN_CHANNEL];
            adjust_gain(gainIndex, gain_delta);
            sendGains = true;
        }
        break;
    }
}

void update_pid_gains(void)
{

    // disable gain changes while radio off
    if (udb_flags._.radio_on)
    {
        // flight_mode and aux2 channels specify pid_gain index and value
        check_flight_mode();
        check_gain_adjust();
    }
}

void run_background_task()
{

    // do stuff which doesn't belong in ISRs
    static unsigned long lastUptime = 0;
    if ((uptime - lastUptime) >= HEARTBEAT_HZ / 20)
    { // at 20 Hz
        lastUptime = uptime;
        throttleUp = (udb_pwIn[THROTTLE_INPUT_CHANNEL] - udb_pwTrim[THROTTLE_INPUT_CHANNEL]) > THROTTLE_DEADBAND;
#if (ENABLE__FAILSAFE)
        // reset value of throttleUp is false
        check_failsafe();
#else
        // without failsafe mux, throttle is always enabled
        udb_throttle_enable = true;
#endif
#if (ENABLE_GAINADJ != 0)
        // call the gain adjustment routine
        update_pid_gains();
#else
        // write gains to eeprom once at startup
        if (writeGains)
        {
            writeGains = false;
            storeGains();
        }
#endif
#if (ENABLE_FLIGHTMODE != 0)
        // check the flight mode switch
        check_flight_mode();
#else
        flight_mode = DEFAULT_FLIGHT_MODE; // force default flight mode
#endif
    }

    // wait for interrupt to save a little power
    // adds 2 cycles of interrupt latency (125 nsec at 16MHz, 50ns at 40MHz)
    Idle();

    return;
}


// Called every 1/2 second at high priority

void udb_background_callback_periodic(void)
{
    if (!didCalibrate)
    {
        // If still calibrating, blink RED
        udb_led_toggle(LED_RED);

        if (udb_flags._.radio_on && dcm_flags._.calib_finished)
        {
            // check LiPo cell count; 1 to 8 cells
            // disable low voltage warning if out of range
            lowVoltageWarning = 0;
            tailFlash = 15;
            int cellCount;
            for (cellCount = 8; cellCount > 0; cellCount--)
            {
                if ((primary_voltage._.W1 > cellCount * 3200) &&
                    (primary_voltage._.W1 <= cellCount * 4200))
                {
                    lowVoltageWarning = cellCount * LVCELL;
                    tailFlash = cellCount;
                    break;
                }
            }

//            // log cellCount
//            snprintf(debug_buffer, sizeof (debug_buffer),
//                     "cellCount: %i, lowVoltageWarning: %u\r\n", cellCount, lowVoltageWarning);
//            log_string(debug_buffer);

#if (HARD_TRIMS == 0)
            // trims not hardwired in udb_init_capture()
            udb_servo_record_trims();
#endif
            // this is called in libDCM.c:dcm_run_init_step()
            //            dcm_calibrate();
            didCalibrate = 1; // not in trunk
            // No longer calibrating: RED off
            LED_RED = LED_OFF;
        }
//        else
//        {
//            // log battery voltage during startup
//            snprintf(debug_buffer, sizeof (debug_buffer),
//                     "primaryV: %05i\r\n", primary_voltage._.W1);
//            log_string(debug_buffer);
//
//
//        }
    }

    return;
}


// Called every time we get gps data (1, 2, or 4 Hz, depending on GPS config)

void dcm_callback_gps_location_updated(void)
{
    return;
}


// Called at HEARTBEAT_HZ, before sending servo pulses

void dcm_servo_callback_prepare_outputs(void)
{
    static int pidCounter = 0;
    static int telCounter = 0;
    static boolean telem_on = false;

    // Update the Green LED to show RC radio status
    if (udb_flags._.radio_on)
    {
        LED_GREEN = LED_ON;
    }
    else
    {
        LED_GREEN = LED_OFF;
    }

    // PID loop at x Hz
    if (++pidCounter >= HEARTBEAT_HZ / PID_HZ)
    {
        pidCounter = 0;
        motorCntrl();
    }
    // don't send telemetry till calibrated
    if (TEL_ALWAYS_ON || didCalibrate)
    {
        if (telem_on && !throttleUp)
        {
            // telemetry was just stopped, store gains and send telemetry
            storeGains();
            sendGains = true;
            telem_on = false;
        }
        // send telemetry if not in failsafe mode, or if gains need recording
        // stops telemetry when failsafe is activated;
        // after .5 second OpenLog will sync its logfile and card may be removed
        if (TEL_ALWAYS_ON || throttleUp || sendGains)
        {
            if (throttleUp) telem_on = true;
            // Serial output at TELEMETRY_HZ
            if (++telCounter >= HEARTBEAT_HZ / TELEMETRY_HZ)
            {
                telCounter = 0;
                send_telemetry();
            }
        }
    }
    return;
}

void udb_callback_radio_did_turn_off(void)
{
}
