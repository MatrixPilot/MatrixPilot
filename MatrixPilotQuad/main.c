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

boolean didCalibrate = 0;

void send_fast_telemetry(void);
void send_telemetry(void);
void motorCntrl(void);
void setup_origin(void);

extern unsigned int pid_gains[];
extern unsigned long uptime;
extern boolean sendGains;
extern boolean sendGPS;

const int max_tilt = (int) (MAX_TILT * .7111); // maximum tilt in byte cicular
int commanded_tilt_gain;

// decoded flight mode input [0,1,2]
int flight_mode = 0;

extern int pitch_step;

// decoded failsafe mux input: true means UDB outputs routed to motors, false means RX throttle to motors
boolean udb_throttle_enable = false;

unsigned int tailFlash = 0;

extern union longww primary_voltage;
extern unsigned int lowVoltageWarning;

int main(void)
{
    // Set up the libraries
    udb_init();
    dcm_init();

    //#warning("GPS yaw drift correction disabled")
    //    dcm_enable_yaw_drift_correction(false);

#if (ENABLE_GAINADJ != 0)
    // read saved gains
    eeprom_SequentialRead(PID_GAINS_BASE_ADDR, (unsigned char *) pid_gains, 6);
#else
    // init gains
    pid_gains[0] = RMAX * TILT_KP;
    pid_gains[1] = RMAX * RATE_KP;
    pid_gains[2] = RMAX * RATE_KD;
    //    pid_gains[3] = 32.0*RMAX*TILT_KI/PID_HZ;
#endif

    //	udb_serial_set_rate(57600) ;
//    udb_serial_set_rate(115200);
    // OpenLog's (calculated) actual baud rate is 222,222 when set up for 230,400
    // minicom set at 230,400 baud works fine with OpenLog at 230,400
    udb_serial_set_rate(222222); // this works with OpenLog set at 230,400 baud

    LED_GREEN = LED_OFF;
    TAIL_LIGHT = LED_OFF; // taillight off

    commanded_tilt_gain = 2 * sine(max_tilt) / 1000;

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
        flight_mode = 2;
    else if (udb_pwIn[FLIGHT_MODE_CHANNEL] < FLIGHT_MODE_THRESH2)
        flight_mode = 1;
    else
        flight_mode = 0;
}

void storeGain(int index)
{
    if (index >= 0 && index < 4)
    {
        // save to EEPROM
        unsigned int address = PID_GAINS_BASE_ADDR + (2 * index);
        eeprom_ByteWrite(address++, (unsigned char) pid_gains[index]);
        eeprom_ByteWrite(address, (unsigned char) (pid_gains[index] >> 8));
    }
}

void storeGains(void)
{
    eeprom_PageWrite(PID_GAINS_BASE_ADDR, (unsigned char*) pid_gains, 8);
}

void adjust_gain(int index, int delta)
{
    if (delta > 0)
    {
        tailFlash = 1;
        delta = RMAX * GAIN_INC;
        if (pid_gains[index] < (0xFFFF - delta))
            pid_gains[index] += delta;
        else
            pid_gains[index] = 0xFFFF;

    }
    else
    {
        tailFlash = 2;
        delta = RMAX * GAIN_INC;
        if (pid_gains[index] > delta)
            pid_gains[index] -= delta;
        else
            pid_gains[index] = 0;
    }
}

// map flight modes [0,1,2] to gain indices
int gainAdjIndex[] = {0, 1, 2};

void check_gain_adjust(void)
{
    static int gainState = -1; // gainState = {-1:init, 0:mode select, 1:adjust gain}
    static int gainIndex = 0;
    static int lastGainChVal = 0;
    // check for gain adjustment; only allow mode adjustment when UDB control disabled
    // To change a gain, turn UDB control off (gear switch down) and set flight mode switch to desired gain index.
    // Gain is adjusted after switching UDB control back on; while UDB control stays on,
    // gain is continually adjusted, gains are logged only on turning UDB control back off.
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
    case 0: // mode select
        //            LED_BLUE = 1;
        if (udb_throttle_enable)
        {
            gainState = 1; // transition to active state
            // initialize lastGainChVal
            lastGainChVal = udb_pwIn[GAIN_CHANNEL];
        }
        break;
    case 1: // adjust gain
        //            LED_BLUE = 0;
        gainIndex = gainAdjIndex[flight_mode];
        if (udb_throttle_enable)
        {
            // while UDB control is still enabled
            // increment or decrement gain value by GAIN_INC, based on change in GAIN_CHANNEL
            int gain_delta = udb_pwIn[GAIN_CHANNEL] - lastGainChVal;
            if (abs(gain_delta) > 20)
            {
                lastGainChVal = udb_pwIn[GAIN_CHANNEL];
                adjust_gain(gainIndex, gain_delta);
            }
        }
        else
        {
            // transition to mode select state and record gain
            gainState = 0;
            storeGain(gainIndex);
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
    static int lastUptime = 0;
    if ((uptime - lastUptime) >= HEARTBEAT_HZ / 20)
    { // at 20 Hz
        lastUptime = uptime;
#if (ENABLE__FAILSAFE)
        // reset value of udb_throttle_enable is false
        check_failsafe();
#else
        // without failsafe mux, throttle is always enabled
        udb_throttle_enable = true;
#endif
#if (ENABLE_GAINADJ != 0)
        // call the gain adjustment routine
        update_pid_gains();
#endif
#if (ENABLE_FLIGHTMODE != 0)
        // check the flight mode switch
        check_flight_mode();
#else
        flight_mode = TILT_MODE;    // force TILT_MODE
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
            // check LiPo cell count; 3S:4S boundary 12.6 : 12.8
            // disable low voltage warning
            lowVoltageWarning = 0;
            tailFlash = 10;
            if ((primary_voltage._.W1 > LVC3S) && (primary_voltage._.W1 < MAXV4S))
            {
                if (primary_voltage._.W1 > MAXV3S)
                {
                    lowVoltageWarning = LVC4S;
                    tailFlash = 4;
                }
                else
                {
                    lowVoltageWarning = LVC3S;
                    tailFlash = 3;
                }
            }

#if (HARD_TRIMS == 0)
            // trims not hardwired in udb_init_capture()
            udb_servo_record_trims();
#endif
            dcm_calibrate();
            didCalibrate = 1;
        }
    }
    else
    {
        // No longer calibrating: solid RED
        //        LED_RED = LED_ON;


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
        // send telemetry if not in failsafe mode, or if gains need recording
        // stops telemetry when failsafe is activated;
        // after .5 second OpenLog will sync its logfile and card may be removed
        if (TEL_ALWAYS_ON || udb_throttle_enable || sendGains)
        {
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
