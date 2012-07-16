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

#if (BOARD_TYPE == UDB4_BOARD)

//	Measure the pulse widths of the servo channel inputs from the radio.
//	The dsPIC makes this rather easy to do using its capture feature.

//	One of the channels is also used to validate pulse widths to detect loss of radio.

//	The pulse width inputs can be directly converted to units of pulse width outputs to control
//	the servos by simply dividing by 2.

int udb_pwIn[NUM_INPUTS + 1]; // pulse widths of radio inputs
int udb_pwTrim[NUM_INPUTS + 1]; // initial pulse widths for trimming

int failSafePulses = 0;


#if (USE_PPM_INPUT != 1)
unsigned int rise[NUM_INPUTS + 1]; // rising edge clock capture for radio inputs

#else
#define MIN_SYNC_PULSE_WIDTH 7000	// 3.5ms
unsigned int rise_ppm; // rising edge clock capture for PPM radio input
#endif

void udb_init_capture(void) {

    int i;
#if (HARD_TRIMS != 0)
#warning("initial udb_pwTrim values set to NEUTRAL_TRIM and THROTTLE_IDLE")
    for (i = 0; i <= NUM_INPUTS; i++) {
        udb_pwIn[i] = 0;
        udb_pwTrim[i] = NEUTRAL_TRIM;
    }
    udb_pwTrim[THROTTLE_INPUT_CHANNEL] = THROTTLE_IDLE;
#else
    // At the end of the calibration interval, udb_servo_record_trims is called to set
    // udb_pwTrim values to whatever is coming from the receiver at that instant.
    // This will be about 1500usec if the TX trims are centered.

    // trim values of zero are never correct for channels 1-4; init to 1500usec instead
    //FIXME for channels 5-8, trim values are unused in MPQpid
    for (i = 0; i <= NUM_INPUTS; i++) {
        udb_pwIn[i] = 0;
        udb_pwTrim[i] = 3000;
    }
    udb_pwTrim[THROTTLE_INPUT_CHANNEL] = 2000;
#endif

    TMR2 = 0; // initialize timer
    //        PR2 = 0xFFFF ;                       // value on reset, full 16 bit count
    T2CONbits.TCKPS = 1; // prescaler = 8 option: count rate 5MHz at 40mips
    // max pulse width suppported = 65535/40E6 = 13.1msec
    T2CONbits.TCS = 0; // use the internal clock
    T2CONbits.TON = 1; // turn on timer 2

    //	configure the capture pins
    IC1CONbits.ICTMR = 1; // use timer 2
    IC1CONbits.ICM = 1; // capture every edge
    _TRISD8 = 1;
    _IC1IP = 6;
    _IC1IF = 0;
    if (NUM_INPUTS > 0) _IC1IE = 1;

#if (USE_PPM_INPUT != 1)
    IC2CONbits.ICTMR = 1; // use timer 2
    IC2CONbits.ICM = 1; // capture every edge

    IC3CONbits.ICTMR = 1; // use timer 2
    IC3CONbits.ICM = 1; // capture every edge

    IC4CONbits.ICTMR = 1; // use timer 2
    IC4CONbits.ICM = 1; // capture every edge

    IC5CONbits.ICTMR = 1; // use timer 2
    IC5CONbits.ICM = 1; // capture every edge

    IC6CONbits.ICTMR = 1; // use timer 2
    IC6CONbits.ICM = 1; // capture every edge

    // configure channel 7 for EagleTree Brushless RPM sensor
    IC7CONbits.ICTMR = 1; // use timer 2
    IC7CONbits.ICM = 0b011; // rising edge mode
    IC7CONbits.ICI = 0b01; // interrupt on every other capture

    IC8CONbits.ICTMR = 1; // use timer 2
    IC8CONbits.ICM = 1; // capture every edge

    _TRISD9 = 1; _TRISD10 = 1; _TRISD11 = 1; _TRISD12 = 1; _TRISD13 = 1; _TRISD14 = 1; _TRISD15 = 1;

    //	set the interrupt priorities to 6
    _IC2IP = 6; _IC3IP = 6; _IC4IP = 6; _IC5IP = 6; _IC6IP = 6; _IC7IP = 6; _IC8IP = 6;

    //	clear the interrupts:
    _IC2IF = 0; _IC3IF = 0; _IC4IF = 0; _IC5IF = 0; _IC6IF = 0; _IC7IF = 0; _IC8IF = 0;

    //	enable the interrupts:
    if (NUM_INPUTS > 1) _IC2IE = 1;
    if (NUM_INPUTS > 2) _IC3IE = 1;
    if (NUM_INPUTS > 3) _IC4IE = 1;
    if (NUM_INPUTS > 4) _IC5IE = 1;
    if (NUM_INPUTS > 5) _IC6IE = 1;
    if (NUM_INPUTS > 6) _IC7IE = 1;
    if (NUM_INPUTS > 7) _IC8IE = 1;
#endif

    return;
}


#if (USE_PPM_INPUT != 1)
// make pwIn values independent of clock and timer rates
#define PWINSCALE (65536 * 32E6 / FREQOSC)

void set_pwIn(int channel, int pw) {
    union longww pww;
    pww.WW = __builtin_muluu(pw, (int) PWINSCALE);
    udb_pwIn[channel] = pww._.W1;
}

// Input Channel 1

void __attribute__((__interrupt__, __no_auto_psv__)) _IC1Interrupt(void) {
    indicate_loading_inter;
    interrupt_save_set_corcon;

    unsigned int time;
    _IC1IF = 0; // clear the interrupt
    while (IC1CONbits.ICBNE) {
        time = IC1BUF;
    }

#if ( NORADIO != 1 )
    if (PORTDbits.RD8) {
        rise[1] = time;
    } else {
        set_pwIn(1, time - rise[1]);

#if ( FAILSAFE_INPUT_CHANNEL == 1 )
        if ((udb_pwIn[FAILSAFE_INPUT_CHANNEL] > FAILSAFE_INPUT_MIN) && (udb_pwIn[FAILSAFE_INPUT_CHANNEL] < FAILSAFE_INPUT_MAX)) {
            failSafePulses++;
        } else {
            failSafePulses = 0;
            udb_flags._.radio_on = 0;
            LED_GREEN = LED_OFF;
        }
#endif

    }
#endif

    interrupt_restore_corcon;
    return;
}


// Input Channel 2

void __attribute__((__interrupt__, __no_auto_psv__)) _IC2Interrupt(void) {
    indicate_loading_inter;
    interrupt_save_set_corcon;

    unsigned int time;
    _IC2IF = 0; // clear the interrupt
    while (IC2CONbits.ICBNE) {
        time = IC2BUF;
    }

#if ( NORADIO != 1 )
    if (PORTDbits.RD9) {
        rise[2] = time;
    } else {
        set_pwIn(2, time - rise[2]);

#if ( FAILSAFE_INPUT_CHANNEL == 2 )
        if ((udb_pwIn[FAILSAFE_INPUT_CHANNEL] > FAILSAFE_INPUT_MIN) && (udb_pwIn[FAILSAFE_INPUT_CHANNEL] < FAILSAFE_INPUT_MAX)) {
            failSafePulses++;
        } else {
            failSafePulses = 0;
            udb_flags._.radio_on = 0;
            LED_GREEN = LED_OFF;
        }
#endif

    }
#endif

    interrupt_restore_corcon;
    return;
}


// Input Channel 3

void __attribute__((__interrupt__, __no_auto_psv__)) _IC3Interrupt(void) {
    indicate_loading_inter;
    interrupt_save_set_corcon;

    unsigned int time;
    _IC3IF = 0; // clear the interrupt
    while (IC3CONbits.ICBNE) {
        time = IC3BUF;
    }

#if ( NORADIO != 1 )
    if (PORTDbits.RD10) {
        rise[3] = time;
    } else {
        set_pwIn(3, time - rise[3]);

#if ( FAILSAFE_INPUT_CHANNEL == 3 )
        if ((udb_pwIn[FAILSAFE_INPUT_CHANNEL] > FAILSAFE_INPUT_MIN) && (udb_pwIn[FAILSAFE_INPUT_CHANNEL] < FAILSAFE_INPUT_MAX)) {
            failSafePulses++;
        } else {
            failSafePulses = 0;
            udb_flags._.radio_on = 0;
            LED_GREEN = LED_OFF;
        }
#endif

    }
#endif

    interrupt_restore_corcon;
    return;
}


// Input Channel 4

void __attribute__((__interrupt__, __no_auto_psv__)) _IC4Interrupt(void) {
    indicate_loading_inter;
    interrupt_save_set_corcon;

    unsigned int time;
    _IC4IF = 0; // clear the interrupt
    while (IC4CONbits.ICBNE) {
        time = IC4BUF;
    }

#if ( NORADIO != 1 )
    if (PORTDbits.RD11) {
        rise[4] = time;
    } else {
        set_pwIn(4, time - rise[4]);

#if ( FAILSAFE_INPUT_CHANNEL == 4 )
        if ((udb_pwIn[FAILSAFE_INPUT_CHANNEL] > FAILSAFE_INPUT_MIN) && (udb_pwIn[FAILSAFE_INPUT_CHANNEL] < FAILSAFE_INPUT_MAX)) {
            failSafePulses++;
        } else {
            failSafePulses = 0;
            udb_flags._.radio_on = 0;
            LED_GREEN = LED_OFF;
        }
#endif

    }
#endif

    interrupt_restore_corcon;
    return;
}


// Input Channel 5

void __attribute__((__interrupt__, __no_auto_psv__)) _IC5Interrupt(void) {
    indicate_loading_inter;
    interrupt_save_set_corcon;

    unsigned int time;
    _IC5IF = 0; // clear the interrupt
    while (IC5CONbits.ICBNE) {
        time = IC5BUF;
    }

#if ( NORADIO != 1 )
    if (PORTDbits.RD12) {
        rise[5] = time;
    } else {
        set_pwIn(5, time - rise[5]);

#if ( FAILSAFE_INPUT_CHANNEL == 5 )
        if ((udb_pwIn[FAILSAFE_INPUT_CHANNEL] > FAILSAFE_INPUT_MIN) && (udb_pwIn[FAILSAFE_INPUT_CHANNEL] < FAILSAFE_INPUT_MAX)) {
            failSafePulses++;
        } else {
            failSafePulses = 0;
            udb_flags._.radio_on = 0;
            LED_GREEN = LED_OFF;
        }
#endif

    }
#endif

    interrupt_restore_corcon;
    return;
}


// Input Channel 6

void __attribute__((__interrupt__, __no_auto_psv__)) _IC6Interrupt(void) {
    indicate_loading_inter;
    interrupt_save_set_corcon;

    unsigned int time;
    _IC6IF = 0; // clear the interrupt
    while (IC6CONbits.ICBNE) {
        time = IC6BUF;
    }

#if ( NORADIO != 1 )
    if (PORTDbits.RD13) {
        rise[6] = time;
    } else {
        set_pwIn(6, time - rise[6]);

#if ( FAILSAFE_INPUT_CHANNEL == 6 )
        if ((udb_pwIn[FAILSAFE_INPUT_CHANNEL] > FAILSAFE_INPUT_MIN) && (udb_pwIn[FAILSAFE_INPUT_CHANNEL] < FAILSAFE_INPUT_MAX)) {
            failSafePulses++;
        } else {
            failSafePulses = 0;
            udb_flags._.radio_on = 0;
            LED_GREEN = LED_OFF;
        }
#endif

    }
#endif

    interrupt_restore_corcon;
    return;
}


// Input Channel 7
// RPM sensor input; measure period of square wave
// one interrupt per brushless motor commutation; 7*RPM for 14 pole motor
// max frequency assuming 10K RPM is 7 * 10K/60 = 1166 Hz, period 857 usec
// if hover RPM is 5K, typical interrupt period is 3.4 msec
int boxCount = 0, filtPer = 0;

void __attribute__((__interrupt__, __no_auto_psv__)) _IC7Interrupt(void) {
    indicate_loading_inter;
    interrupt_save_set_corcon;

    _IC7IF = 0; // clear the interrupt

#if (ENABLE_RPM_SENSOR != 0)
    // run an N sample boxcar integrator with outlier rejection
    // at 11K RPM and 14 poles, commutation freuency is 1283 Hz
    // with period of 1558 counts. 1558 is therefore the minimum valid period
    // The lower limit of rpm is set by the period of the timer; this is 2^16/2E6 = 32.768msec
    // This period corresponds to 261 rpm for a 14 pole motor (1800 rpm for a 2 pole)
    // allowing 3 bits of headroom for integration, min rpm is 2092
    unsigned int t1, t2, delta;
    t1 = IC7BUF;
    t2 = IC7BUF;
    delta = t2 - t1;
    if ((delta > 1558) && (delta < 16384)) {
        boxCount++;
        filtPer += delta;
    }
    if (boxCount >= 4) {
        udb_pwIn[7] = (filtPer >> 2);
        boxCount = 0;
        filtPer = 0;
    }
#endif
    interrupt_restore_corcon;
    return;
}


// Input Channel 8

void __attribute__((__interrupt__, __no_auto_psv__)) _IC8Interrupt(void) {
    indicate_loading_inter;
    interrupt_save_set_corcon;

    unsigned int time;
    _IC8IF = 0; // clear the interrupt
    while (IC8CONbits.ICBNE) {
        time = IC8BUF;
    }

#if ( NORADIO != 1 )
    if (PORTDbits.RD15) {
        rise[8] = time;
    } else {
        set_pwIn(8, time - rise[8]);

#if ( FAILSAFE_INPUT_CHANNEL == 8 )
        if ((udb_pwIn[FAILSAFE_INPUT_CHANNEL] > FAILSAFE_INPUT_MIN) && (udb_pwIn[FAILSAFE_INPUT_CHANNEL] < FAILSAFE_INPUT_MAX)) {
            failSafePulses++;
        } else {
            failSafePulses = 0;
            udb_flags._.radio_on = 0;
            LED_GREEN = LED_OFF;
        }
#endif

    }
#endif

    interrupt_restore_corcon;
    return;
}

#else // #if (USE_PPM_INPUT == 1)

#if (PPM_SIGNAL_INVERTED == 1)
#define PPM_PULSE_VALUE 0
#else
#define PPM_PULSE_VALUE 1
#endif

unsigned char ppm_ch = 0;

// PPM Input on Channel 1

void __attribute__((__interrupt__, __no_auto_psv__)) _IC1Interrupt(void) {
    indicate_loading_inter;
    interrupt_save_set_corcon;

    unsigned int time;
    _IC1IF = 0; // clear the interrupt
    while (IC1CONbits.ICBNE) {
        time = IC1BUF;
    }

#if ( NORADIO != 1 )

    if (_RD8 == PPM_PULSE_VALUE) {
        unsigned int pulse = time - rise_ppm;
        rise_ppm = time;

        if (pulse > MIN_SYNC_PULSE_WIDTH) //sync pulse
        {
            ppm_ch = 1;
        } else {
            if (ppm_ch > 0 && ppm_ch <= PPM_NUMBER_OF_CHANNELS) {
                if (ppm_ch <= NUM_INPUTS) {
                    udb_pwIn[ppm_ch] = pulse;

                    if (ppm_ch == FAILSAFE_INPUT_CHANNEL && udb_pwIn[FAILSAFE_INPUT_CHANNEL] > FAILSAFE_INPUT_MIN && udb_pwIn[FAILSAFE_INPUT_CHANNEL] < FAILSAFE_INPUT_MAX) {
                        failSafePulses++;
                    }
                }
                ppm_ch++; //scan next channel
            }
        }
    }
#endif

    interrupt_restore_corcon;
    return;
}

#endif

#endif
