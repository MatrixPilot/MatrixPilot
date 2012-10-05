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


#include "libUDB_internal.h"

#if(USE_I2C1_DRIVER == 1)
#include "I2C.h"
#include "events.h"
#endif

// Include the NV memory services if required
#if(USE_NV_MEMORY == 1)
#include "NV_memory.h"
//#include "data_storage.h"
//#include "data_services.h"
#endif

#if (BOARD_IS_CLASSIC_UDB == 1)
#if ( CLOCK_CONFIG == CRYSTAL_CLOCK )
#define CPU_LOAD_PERCENT	16*400   // = (100 / (8192 * 2)) * (256**2)
#elif ( CLOCK_CONFIG == FRC8X_CLOCK )
#define CPU_LOAD_PERCENT	16*109   // = ((100 / (8192 * 2)) * (256**2))/3.6864
#endif

#elif ((BOARD_TYPE == UDB4_BOARD) || (BOARD_TYPE & AUAV2_BOARD))
// define CPU_RES so that 1 count is .01% => 10,000 counts = 100%
#define CPU_RES (FREQOSC / 20000)
// define CPU_LOAD_PERCENT to return units of percent
#define CPU_LOAD_PERCENT ((65536.0 * 100.0 * CPU_RES * 2) / FREQOSC)
#endif


unsigned int cpu_timer = 0;
unsigned int _cpu_timer = 0;

unsigned int idle_timer = 0;
unsigned int _idle_timer = 0;

// Local elapsed time from boot (in heartbeats), used for timestamping.
// rolls over at 2^32 counts: interval is 497 days at 100Hz
unsigned long uptime = 0;

extern union longww primary_voltage;
extern unsigned int lowVoltageWarning;

// decoded failsafe mux input: true means UDB outputs routed to motors, false means RX throttle to motors
extern boolean udb_throttle_enable;

// flag to control tail light
extern unsigned int tailFlash;
extern boolean didCalibrate;

unsigned int udb_heartbeat_counter = 0;

void udb_run_init_step(void);


#if ((BOARD_TYPE == UDB4_BOARD) || (BOARD_TYPE & AUAV2_BOARD))
#define _TTRIGGERIP _T7IP
#define _TTRIGGERIF _T7IF
#define _TTRIGGERIE _T7IE
#else
#define _TTRIGGERIP _T3IP
#define _TTRIGGERIF _T3IF
#define _TTRIGGERIE _T3IE
#endif

#if ((BOARD_TYPE == UDB4_BOARD) || (BOARD_TYPE & AUAV2_BOARD))
#define _THEARTBEATIP _T6IP
#define _THEARTBEATIF _T6IF
#define _THEARTBEATIE _T6IE
#else
#define _THEARTBEATIP _PWMIP
#define _THEARTBEATIF _PWMIF
#define _THEARTBEATIE _PWMIE
#endif

void udb_init_clock(void) /* initialize timers */ {

    TRISF = 0b1111111111101100;

#if(USE_I2C1_DRIVER == 1)
	init_events();
	I2C1_init();
#endif

#if(USE_NV_MEMORY == 1)
	nv_memory_init();
//	data_storage_init();
//	data_services_init();
#endif


    // Initialize timer1, used as the HEARTBEAT_HZ heartbeat of libUDB.
    TMR1 = 0;
#if ((BOARD_TYPE == UDB4_BOARD) || (BOARD_TYPE & AUAV2_BOARD))
    // clock is 40MHz max: prescaler = 8, timer clock at 5MHz, PR1 = 5e6/100 = 50,000 < 65,535
    T1CONbits.TCKPS = 1;
    PR1 = (FREQOSC / (8 * CLK_PHASES)) / HEARTBEAT_HZ; // period 1/HEARTBEAT_HZ
#elif ( CLOCK_CONFIG == CRYSTAL_CLOCK )
#error ("code not yet using HEARTBEAT_HZ")
    PR1 = 12500; // 25 millisecond period at 16 Mz clock, inst. prescale 4, tmr prescale 8
    T1CONbits.TCKPS = 1; // prescaler = 8
#elif ( CLOCK_CONFIG == FRC8X_CLOCK )
#error ("code not yet using HEARTBEAT_HZ")
    PR1 = 46080; // 25 millisecond period at 58.982 Mz clock,inst. prescale 4, tmr prescale 8
    T1CONbits.TCKPS = 1; // prescaler = 8
#endif
    T1CONbits.TCS = 0; // use the crystal to drive the clock
    _T1IP = 6; // High priority
    _T1IF = 0; // clear the interrupt

#if ((BOARD_TYPE & AUAV2_BOARD) == 0) && (DUAL_IMU != 1)
    // AUAV2 and dualIMU UDB4 use MPU6000 interrupt for heartbeat instead of Timer1
    _T1IE = 1; // enable the interrupt
#endif

    T1CONbits.TON = 1; // turn on timer 1

    //    // Set up Timer 4
    T4CONbits.TON = 0; // Disable Timer
    //    TMR4 = 0x00; // Clear timer register
    //    //    PR4 = 0xFFFF;   // period 2^16 cycles (reset value)
    //    PR4 = CPU_RES - 1; // measure instruction cycles in units of CPU_RES
    //    _idle_timer = 0; // initialize the load counter
    //    T4CONbits.TCKPsnaS = 0; // prescaler = 1
    //    T4CONbits.TGATE = 0; // not gated
    //    T4CONbits.TCS = 0; // Select internal instruction cycle clock
    //    _T4IP = 6;
    //    _T4IF = 0;
    //    _T4IE = 1;

    // set up SCL2, SDA2 as outputs for monitoring interrupts
    //    I2C2CONbits.I2CEN = 0;
    //    _TRISA2 = 0;
    //    _TRISA3 = 0;
    //    SCL2 = 0;
    //    SDA2 = 0;

    // use timer 5 as Fcy precision interval timer
    // at Fcy=40MHz T5 resolution is 25ns with rollover every 1.6msec
    // Timer 5 will be started on entry to each ISR and stopped on return from each ISR
    TMR5 = 0; // initialize timer
    PR5 = CPU_RES - 1; // measure instruction cycles in units of CPU_RES
    //    _cpu_timer = 0; // initialize the load counter
    T5CONbits.TCKPS = 0; // no prescaler
    T5CONbits.TCS = 0; // Select internal instruction cycle clock
    _T5IP = 6; // high priority, but ISR is very short
    _T5IF = 0; // clear the interrupt
    _T5IE = 1; // enable the interrupt
    T5CONbits.TON = 0; // turn off timer 5 until we enter an interrupt


    // The TTRIGGER interrupt (T3 or T7 depending on the board) is used to
    // trigger background tasks such as navigation processing after binary data
    // is received from the GPS.
    _TTRIGGERIP = 2; // priority 2
    _TTRIGGERIF = 0; // clear the interrupt
    _TTRIGGERIE = 1; // enable the interrupt


    // Start the PWM Interrupt, but not the PWM timer.
    // This is used as a trigger from the high priority heartbeat ISR to
    // start all the HEARTBEAT_HZ processing at a lower priority.
    _THEARTBEATIF = 0; // clear the PWM interrupt
    _THEARTBEATIP = 3; // priority 3
#if ((BOARD_TYPE != UDB4_BOARD) && ((BOARD_TYPE & AUAV2_BOARD) == 0))
    _PEN1L = _PEN2L = _PEN3L = 0; // low pins used as digital I/O
    _PEN1H = _PEN2H = _PEN3H = 0; // high pins used as digital I/O
#endif
    _THEARTBEATIE = 1; // enable the PWM interrupt

    return;
}

#if (BOARD_TYPE & AUAV2_BOARD) || (DUAL_IMU == 1)
// The Heartbeat of libUDB is the MPU6000 interrupt

void doT1Interrupt(void) {

    indicate_loading_inter; // for cpu loading measurement

    static boolean secToggle = true;
    static int twoHzCounter = 0;

    // set the motor PWM values; these are sent to all ESCs continuously at ESC_HZ
    udb_set_dc();

    // Call the periodic callback at 2Hz
    if (++twoHzCounter >= (HEARTBEAT_HZ / 2)) {
        twoHzCounter = 0;

        udb_background_callback_periodic();

        // Capture cpu_timer once per second.
        if ((secToggle = !secToggle)) { // the assignment is intentional
            T5CONbits.TON = 0; // turn off timer 5
            cpu_timer = _cpu_timer; // snapshot the load counter
            _cpu_timer = 0; // reset the load counter
            T5CONbits.TON = 1; // turn on timer 5
        }
    }


    // Trigger the HEARTBEAT_HZ calculations, but at a lower priority
    _THEARTBEATIF = 1;

    uptime++;
    udb_heartbeat_counter++;

    return;
}


#else

// This high priority interrupt is the Heartbeat of libUDB.

void __attribute__((__interrupt__, __no_auto_psv__)) _T1Interrupt(void) {

    indicate_loading_inter;
    interrupt_save_set_corcon;

    static boolean secToggle = true;
    static int twoHzCounter = 0;

    _T1IF = 0; // clear the interrupt

    //    SCL2 = 1 - SCL2;

    // set the motor PWM values; these are sent to all ESCs continuously at ESC_HZ
    udb_set_dc();

    // Call the periodic callback at 2Hz
    if (++twoHzCounter >= (HEARTBEAT_HZ / 2)) {
        twoHzCounter = 0;

        udb_background_callback_periodic();

        // Capture cpu_timer once per second.
        if ((secToggle = !secToggle)) {
            T5CONbits.TON = 0; // turn off timer 5
            cpu_timer = _cpu_timer; // snapshot the load counter
            _cpu_timer = 0; // reset the load counter
            T5CONbits.TON = 1; // turn on timer 5

            //            T4CONbits.TON = 0; // turn off timer 4
            //            idle_timer = _idle_timer; // snapshot the idle counter
            //            _idle_timer = 0; // reset the idle counter
            //            T4CONbits.TON = 1; // turn on timer 4
        }
    }


    // Trigger the HEARTBEAT_HZ calculations, but at a lower priority
    _THEARTBEATIF = 1;

    uptime++;
    udb_heartbeat_counter++;

    interrupt_restore_corcon;
    return;
}
#endif

// Trigger the TRIGGER interrupt.

void udb_background_trigger(void) {
    _TTRIGGERIF = 1; // trigger the interrupt
    return;
}


// Process the TRIGGER interrupt.
// This is used by libDCM to kick off gps-based calculations at a lower
// priority after receiving each new set of GPS data.
#if ((BOARD_TYPE == UDB4_BOARD) || (BOARD_TYPE & AUAV2_BOARD))
void __attribute__((__interrupt__, __no_auto_psv__)) _T7Interrupt(void)
#else

void __attribute__((__interrupt__, __no_auto_psv__)) _T3Interrupt(void)
#endif
{
    indicate_loading_inter;
    interrupt_save_set_corcon;

    _TTRIGGERIF = 0; // clear the interrupt

    if ((BOARD_TYPE & AUAV2_BOARD) == 0)
        udb_background_callback_triggered();

    interrupt_restore_corcon;
    return;
}

unsigned char udb_cpu_load(void) {
    // scale cpu_timer to seconds*100 for percent loading
    return (unsigned char) (__builtin_muluu(cpu_timer, CPU_LOAD_PERCENT) >> 16);
}

//void __attribute__((__interrupt__, __no_auto_psv__)) _T4Interrupt(void) {
//    interrupt_save_set_corcon;
//
//    TMR4 = 0; // reset the timer
//    _idle_timer++; // increment the load counter
//    _T4IF = 0; // clear the interrupt
//
//    interrupt_restore_corcon;
//    return;
//}

void __attribute__((__interrupt__, __no_auto_psv__)) _T5Interrupt(void) {
    interrupt_save_set_corcon;

    TMR5 = 0; // reset the timer
    _cpu_timer++; // increment the load counter
    _T5IF = 0; // clear the interrupt

    interrupt_restore_corcon;
    return;
}

int loopCounter = 0;
//	Executes whatever lower priority calculation needs to be done every heartbeat.
//	This is a good place to eventually compute pulse widths for servos.
#if ((BOARD_TYPE == UDB4_BOARD) || (BOARD_TYPE & AUAV2_BOARD))
void __attribute__((__interrupt__, __no_auto_psv__)) _T6Interrupt(void)
#else

void __attribute__((__interrupt__, __no_auto_psv__)) _PWMInterrupt(void)
#endif
{
    indicate_loading_inter;
    interrupt_save_set_corcon;

    _THEARTBEATIF = 0; /* clear the interrupt */

    loopCounter++;

#if ( NORADIO != 1 )
    // 20Hz testing for radio link
    if (loopCounter >= (HEARTBEAT_HZ / 20)) {
        loopCounter = 0;
        if (failSafePulses == 0) {
            if (udb_flags._.radio_on == 1) {
                udb_flags._.radio_on = 0;
                udb_callback_radio_did_turn_off();
                LED_GREEN = LED_OFF;
            }
        } else {
            udb_flags._.radio_on = 1;
            LED_GREEN = LED_ON;
        }
        failSafePulses = 0;

        // test for low voltage and failsafe and annunciation
        if (
            (didCalibrate && !udb_throttle_enable && ((udb_pwIn[THROTTLE_INPUT_CHANNEL] - udb_pwTrim[THROTTLE_INPUT_CHANNEL] > THROTTLE_DEADBAND))) ||
            (primary_voltage._.W1 < lowVoltageWarning) ||
            (tailFlash > 0)
            ) {

            TAIL_LIGHT = 1 - TAIL_LIGHT;
            LED_BLUE = 1 - LED_BLUE;
            if ((tailFlash > 0) && TAIL_LIGHT) tailFlash--;
        } else {
            TAIL_LIGHT = LED_OFF;
            LED_BLUE = LED_OFF;
        }
    }
#endif

#ifdef VREF
    vref_adj = (udb_vref.offset >> 1) - (udb_vref.value >> 1);
#else
    vref_adj = 0;
#endif

    calculate_analog_sensor_values();
    udb_callback_read_sensors();
    udb_flags._.a2d_read = 1; // signal the A/D to start the next summation

    udb_servo_callback_prepare_outputs();

    interrupt_restore_corcon;
    return;
}
