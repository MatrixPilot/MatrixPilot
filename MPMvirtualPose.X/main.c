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

#include "libDCM.h"
#include "options.h"
#include "motorCntrl.h"
#include "defines.h"

#if (BOARD_TYPE == AUAV2_BOARD_ALPHA1)
void parseSbusData(void);
extern boolean sbusDAV;
#endif

boolean didCalibrate = false;
boolean callSendTelemetry = false;

void send_fast_telemetry(void);
void send_telemetry(void);
void setup_origin(void);

extern int accb_cnt;
extern unsigned int pid_gains[];
extern unsigned long uptime;
extern boolean sendGains;
extern boolean sendGPS;
extern char debug_buffer[256];
void log_string(char*);
void queue_data(char*, int);

// decoded flight mode switch
int flight_mode = 0;

// decoded gain mode switch
int gainadj_mode = 0;

extern int pitch_step;

// decoded failsafe mux input: true means UDB outputs routed to motors, false means RX throttle to motors
boolean udb_throttle_enable = false;

boolean throttleUp = false;

boolean writeGains = false;

unsigned int tailFlash = 0;

extern union longww primary_voltage;
extern unsigned int lowVoltageWarning;

void storeGains(void) {

#if (BOARD_TYPE == UDB4_BOARD)
    int index;
    for (index = 0; index < PID_GAINS_N; index++) {
        // save to EEPROM
        unsigned int address = PID_GAINS_BASE_ADDR + (2 * index);
        eeprom_ByteWrite(address++, (unsigned char) pid_gains[index]);
        eeprom_ByteWrite(address, (unsigned char) (pid_gains[index] >> 8));
    }
    //    eeprom_PageWrite(PID_GAINS_BASE_ADDR, (unsigned char*) pid_gains, 2 * PID_GAINS_N);
#else
    return;
#endif
}

int main(void) {
    // Set up the libraries
    udb_init();
    dcm_init();
    init_aircraft();

    //#warning("GPS yaw drift correction disabled")
    //    dcm_enable_yaw_drift_correction(false);

#if (ENABLE_GAINADJ != 0)
    // read saved gains
    boolean status = false;
    while (!status) {
        status = eeprom_SequentialRead(PID_GAINS_BASE_ADDR, (unsigned char *) pid_gains, 2 * PID_GAINS_N);
        //        status = false;
    }
#else
    // init gains
    pid_gains[ROLL_KP_INDEX] = (unsigned int) (RMAX * ROLL_KP);
    pid_gains[PITCH_KP_INDEX] = (unsigned int) (RMAX * PITCH_KP);
    pid_gains[RRATE_KP_INDEX] = (unsigned int) (RMAX * RRATE_KP);
    pid_gains[RRATE_KD_INDEX] = (unsigned int) (RMAX * RRATE_KD);
    pid_gains[PRATE_KP_INDEX] = (unsigned int) (RMAX * PRATE_KP);
    pid_gains[PRATE_KD_INDEX] = (unsigned int) (RMAX * PRATE_KD);
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

#if (SERIAL_OUTPUT_FORMAT == SERIAL_MAVLINK)
    init_serial();
#else
    // OpenLog's (calculated) actual baud rate is 222,222 when set up for 230,400
    // minicom set at 230,400 baud works fine with OpenLog at 230,400
    udb_serial_set_rate(TELEMETRY_BAUD); // this works with OpenLog set at 230,400 baud
#endif
    
    LED_GREEN = LED_OFF;
    tail_light_off();

    // Start it up!
    udb_run(); // This never returns.

    return 0;
}

void check_failsafe() {
    // decode flight mode input
    if (udb_pwIn[FAILSAFE_MUX_CHANNEL] < FAILSAFE_MUX_THRESH)
        udb_throttle_enable = true;
    else
        udb_throttle_enable = false;
}

void check_flight_mode(void) {
    // decode flight mode input
    if (udb_pwIn[MODE_SWITCH_INPUT_CHANNEL] < MODE_SWITCH_THRESHOLD_LOW) {
        flight_mode = FLIGHT_MODE_2;
        gainadj_mode = 2;
    } else if (udb_pwIn[MODE_SWITCH_INPUT_CHANNEL] < MODE_SWITCH_THRESHOLD_HIGH) {
        flight_mode = FLIGHT_MODE_1;
        gainadj_mode = 1;
    } else {
        flight_mode = FLIGHT_MODE_0;
        gainadj_mode = 0;
    }
}

#if (ENABLE_GAINADJ == 1)

void storeGain(int index) {
    if (index >= 0 && index < PID_GAINS_N) {
        // save to EEPROM
        unsigned int address = PID_GAINS_BASE_ADDR + (2 * index);
        eeprom_ByteWrite(address++, (unsigned char) pid_gains[index]);
        eeprom_ByteWrite(address, (unsigned char) (pid_gains[index] >> 8));
    }
}

void adjust_gain(int index, int delta) {
    if (delta > 0) {
        delta = RMAX * GAIN_INC;
        if (pid_gains[index] < (0xFFFF - delta)) {
            tailFlash = 1;
            pid_gains[index] += delta;
        } else {
            tailFlash = 5;
            pid_gains[index] = 0xFFFF;
        }

    } else {
        delta = RMAX * GAIN_INC;
        if (pid_gains[index] > delta) {
            tailFlash = 2;
            pid_gains[index] -= delta;
        } else {
            tailFlash = 5;
            pid_gains[index] = 0;
        }
    }
}

// map flight modes [0,1,2] to gain indices
int gainAdjIndex[] = {ADJ_GAIN_0, ADJ_GAIN_1, ADJ_GAIN_2};

void check_gain_adjust(void) {
    static int gainState = -1; // gainState = {-1:init, 0:mode select, 1:adjust gain}
    static int gainIndex = 0;
    static int lastGainChVal = 0;
    // To change a gain, set flight mode switch to desired gain adjust index.
    // gain is adjusted, whenever GAIN_CHANNEL changes by more than 6 PWM counts.
    switch (gainState) {
        case -1: // init
            if (dcm_flags._.calib_finished) {
                // initialize lastGainChVal after cal complete
                lastGainChVal = udb_pwIn[GAIN_CHANNEL];
                gainState = 0;
            }
            break;
        case 0: // adjust gain
            gainIndex = gainAdjIndex[gainadj_mode];
            // increment or decrement gain value by GAIN_INC, based on change in GAIN_CHANNEL
            int gain_delta = udb_pwIn[GAIN_CHANNEL] - lastGainChVal;
            if (abs(gain_delta) > GAIN_DELTA) {
                lastGainChVal = udb_pwIn[GAIN_CHANNEL];
                adjust_gain(gainIndex, gain_delta);
                storeGains();
                sendGains = true;
            }
            break;
    }
}

void update_pid_gains(void) {

    // disable gain changes while radio off
    if (udb_flags._.radio_on) {
        // flight_mode and aux2 channels specify pid_gain index and value
        check_flight_mode();
        check_gain_adjust();
    }
}
#endif

// this method is only called at IPL 0

void run_background_task() {

    static unsigned int lastRadioCheck = 0, lastLightsCheck = 0, slowCount = 0;

    {
        // do non-ISR stuff which needs to happen as frequently as possible
    }

    // at 20Hz do other stuff which doesn't belong in ISRs
    if ((udb_heartbeat_counter - lastRadioCheck) > HEARTBEAT_HZ / 20) {
        lastRadioCheck = uptime;
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
        if (writeGains) {
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

#if (SERIAL_OUTPUT_FORMAT != SERIAL_MAVLINK)
    if (callSendTelemetry) {
        send_telemetry();
        callSendTelemetry = false;
    }
#else
    // for mavlink telemetry, ignore callSendTelemetry and just send gains/GPS
    if (sendGains || sendGPS) {
        send_telemetry();
    }
#endif

    if ((udb_heartbeat_counter - lastLightsCheck) > HEARTBEAT_HZ / 10) {
        lastLightsCheck = udb_heartbeat_counter;

        if (tailFlash > 0) {
            // annunciation mode has highest priority
            if (tail_light_toggle()) {
                // decrement flash count each time tail_light turns off
                tailFlash--;
            }
        } else {
            // either low voltage warning or normal flight mode
            if (primary_voltage._.W1 < lowVoltageWarning) {
                // rapid flash of tail light for low voltage warning has second priority
                tail_light_toggle();
            } else {
                // run lights state machine at lower rate at lowest priority
                if (++slowCount > 2) {
                    slowCount = 0;
                    if (motorsArmed > 2) {
                        if (udb_pwIn[FAILSAFE_MUX_CHANNEL] > FAILSAFE_MUX_THRESH) {
                            lightFSM();
                        } else {
                            all_lights_off();
                        }
                    }
                }
            }
        }
    }

    // start the idle timer and
    // wait for interrupt to save a little power
    // Idle() adds 2 cycles of interrupt latency (125 nsec at 16MHz, 50ns at 40MHz)
    // the disi instruction adds 4 more, worst-case for a total of 150ns at 40MIPS
    __builtin_disi(4);
    T8CONbits.TON = 1;
    Idle();

    return;
}
