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

#include <string.h>
#include "../../libUDB/libUDB_internal.h"
#include "../../libUDB/filters.h"
#include "analog2digital_udb4.h"

#if (BOARD_TYPE == UDB4_BOARD)

//	Analog to digital processing.
//	Sampling and conversion is done automatically, so that all that needs to be done during
//	interrupt processing is to read the data out of the buffer.
//	Raw samples are taken approximately 500 per second per channel.
//	A first order digital lowpass filter with a time constant of about 32 milliseconds
//  is applied to improve signal to noise.

//	Variables.

extern boolean callSendTelemetry;

int accelBuffCount = 0;
long acc_buffer[2][3][BUFFSIZE];
int accPing;

struct ADchannel udb_xaccel, udb_yaccel, udb_zaccel; // x, y, and z accelerometer channels
struct ADchannel udb_xrate, udb_yrate, udb_zrate; // x, y, and z gyro channels
struct ADchannel udb_vref; // reference voltage
struct ADchannel primaryV; // primary battery voltage

#if (NUM_ANALOG_INPUTS >= 1)
struct ADchannel udb_analogInputs[NUM_ANALOG_INPUTS]; // 0-indexed, unlike servo pwIn/Out/Trim arrays
#endif


int vref_adj;

#if (RECORD_FREE_STACK_SPACE == 1)
unsigned int maxstack = 0;
#endif

// dsPIC33FJXXXGPX06A/X08A/X10A
// minimum allowed 12-bit ADC clock period is 118ns or 8.47MHz
// minimum allowed 10-bit ADC clock period is 76ns, or 13.16MHz
// 12 bit mode conversion time is 14 TAD cycles
// total sample/conversion time is (14+SAMC) * TAD
// for 400nsec TAD, total is 10usec for 100KHz conversion rate with SAMC = 11
// *** observed 72usec interval between interrupts on scope => interrupt every sample

// ADCS bits 6 and 7 are reserved; max divisor is 64
// adc_clk = 625KHz
#define ADCLK_DIV_N_MINUS_1 63
// TAD is 1/ADC_CLK
#define ADC_CLK (FREQOSC / (CLK_PHASES * (ADCLK_DIV_N_MINUS_1 + 1)))
#if (ADC_CLK > 8470000)
#error ADC_CLK too fast
#endif
// for automatic sequential sampling and conversion, the sampling time is 
// SAMC=ADSAMP_TIME_N TAD cycles
// *** from 31 to 16=> ADC_RATE = 20.833 KHz
#define ADSAMP_TIME_N 16
//FIXME FREQOSC: ADC_RATE is dependent on clock frequency
// At FREQOSC=40MHz, ADC_CLK=625KHz, at 32MHz:250KHz
// At FREQOSC=40MHz, ADC_RATE=13.9KHz, at 32MHz:5.56KHz
// At 32MHz, per channel rate is about 800Hz and lp2 3dB point is at 30Hz
// At 40MHz: 13.889KHz ADC rate and 8 channels seq. sampled, the per channel rate is
// about 1.7361 KHz and lp2 3dB point is at 45Hz.
// Going from 16 to 40MHz pushes the lowpass filter cutoffs up by 2.5x
// and this may require changing the lp2 filter coefficients to maintain stability.
// With PID loop at 400Hz, should be able to deal with 100Hz bandwidth...
#define ADC_RATE (1.0 * ADC_CLK / (ADSAMP_TIME_N + 14))

// ***from 8 to 10=> scan rate = 10KHz
#define N_CHANNELS_SCANNED 8

char sampcount = 1;

void udb_init_gyros(void) {
    // turn off auto zeroing
    _TRISC4 = 0;
    _TRISB14 = 0;
    _LATC4 = 0;
    _LATB14 = 0;

    return;
}

void udb_init_accelerometer(void) {
    _TRISA6 = 0; // GSELECT is an output
    _LATA6 = 1; // 6 G setting
//    _LATA6 = 0; // 1.5 G setting

    //	set as inputs:
    _TRISB9 = 1;
    _TRISB10 = 1;
    _TRISB11 = 1;

    return;
}

void udb_init_ADC(void) {
    udb_init_gyros();
    udb_init_accelerometer();

    AD2CSSL = 0; // start with no channels selected
    AD2PCFGL = 0b1111111111111111; // start with all digital, set the A/D

    //	include the 110 degree/second scale, gyro
    /*
    AD2CSSLbits.CSS0 = 1 ;
    AD2CSSLbits.CSS3 = 1 ;
    AD2CSSLbits.CSS5 = 1 ;

    AD2PCFGLbits.PCFG0 = 0 ;
    AD2PCFGLbits.PCFG3 = 0 ;
    AD2PCFGLbits.PCFG5 = 0 ;
     */

    //	include the 500 degree/second scale, gyro
    AD2CSSLbits.CSS1 = 1;
    AD2CSSLbits.CSS4 = 1;
    AD2CSSLbits.CSS6 = 1;

    AD2PCFGLbits.PCFG1 = 0;
    AD2PCFGLbits.PCFG4 = 0;
    AD2PCFGLbits.PCFG6 = 0;

    //	include the accelerometer in the scan:
    AD2CSSLbits.CSS9 = 1;
    AD2CSSLbits.CSS10 = 1;
    AD2CSSLbits.CSS11 = 1;

    AD2PCFGLbits.PCFG9 = 0;
    AD2PCFGLbits.PCFG10 = 0;
    AD2PCFGLbits.PCFG11 = 0;

    //      include AN12,15 in the scan
    // WARNING: ADC2 can access only inputs AN0-AN15, AN16-31 are not available
    // This means the PCB pads AN16-18 are unusable with this version of code
    // Simply changing "AD2" to "AD1" will fix the problem
    AD2CSSLbits.CSS12 = 1;
    AD2CSSLbits.CSS15 = 1;
    AD2PCFGLbits.PCFG12 = 0;
    AD2PCFGLbits.PCFG15 = 0;

    AD2CON1bits.AD12B = 1; // 12 bit A to D
    AD2CON1bits.FORM = 3; // signed fractional
    AD2CON1bits.SSRC = 7; // auto convert
    AD2CON1bits.ASAM = 1; // auto samp
    AD2CON1bits.SIMSAM = 0; // multiple channels in sequence

    AD2CON2bits.VCFG = 0; // use supply as reference voltage
    AD2CON2bits.CSCNA = 1; // scanA ch0

    AD2CON2bits.ALTS = 0; // always A
    //	AD2CON2bits.BUFM = 1 ;		// ping-pong buffers

    //	AD2CON3bits.ADCS = 127 ;        // TAD = 8 microseconds (127+1)/16MHz *** NO, bits 7:6 have no effect
    AD2CON3bits.ADCS = ADCLK_DIV_N_MINUS_1; // TAD = (15+1)/(FREQOSC/2) = 0.4usec at 40mips
    AD2CON3bits.SAMC = ADSAMP_TIME_N; // auto sample time = 11 TAD, approximately 4.4 microseconds

    //	AD2CON1bits.ADDMABM = 1 ;	// DMA buffer written in conversion order

    // N_CHANNELS_SCANNED samples ??? this affects both DMA and non-DMA operation
    AD2CON2bits.SMPI = N_CHANNELS_SCANNED - 1;
    //	AD2CON4bits.DMABL = 1 ;		// double buffering

    _AD2IF = 0; // clear the AD interrupt
    _AD2IP = 5; // priority 5
    _AD2IE = 1; // enable the interrupt
    AD2CON1bits.ADON = 1; // turn on the A to D

    return;
}

// 3dB frequency about 45Hz at 2KHz sample rate, 38Hz at fs = 1.7 KHz
#define LPCB (unsigned int)(65536 / 12)

#define ADC2SAMPLE ((int)(ADC2BUF0))

void __attribute__((__interrupt__, __no_auto_psv__)) _ADC2Interrupt(void) {
    static union int32_w2 gx, gy, gz;
    static union int32_w2 ax, ay, az;
    static union int32_w2 pv, rv;
    static int integrationCount = 0;
    int i;

    indicate_loading_inter;
    interrupt_save_set_corcon;

    _AD2IF = 0; // clear the AD interrupt
    _LATD6 = 1;

#if (RECORD_FREE_STACK_SPACE == 1)
    unsigned int stack = WREG15;
    if (stack > maxstack) {
        maxstack = stack;
    }
#endif

#if (HILSIM != 1)
    switch (sampcount) {
        case yrateBUFF:
            udb_yrate.input = ADC2SAMPLE;
            break;

        case zrateBUFF:
            udb_zrate.input = ADC2SAMPLE;
            break;

        case xrateBUFF:
            udb_xrate.input = ADC2SAMPLE;
            break;

        case zaccelBUFF:
            udb_zaccel.input = ADC2SAMPLE;
            break;

        case xaccelBUFF:
            udb_xaccel.input = -ADC2SAMPLE;
            break;

        case yaccelBUFF:
            udb_yaccel.input = -ADC2SAMPLE;
            break;

        case analogInput1BUFF:
            udb_vref.input = ADC2SAMPLE;
            break;

        case analogInput2BUFF:
            primaryV.input = ADC2SAMPLE;
            break;

        default:
            i = ADC2SAMPLE; // Pull the sample, but ignore it
            break;
    }
#else
    (void) ADC2SAMPLE; // HILSIM: Pull the sample, but ignore it
#endif
    sampcount++;
    if (sampcount > N_CHANNELS_SCANNED) {
        sampcount = 1;

        // apply lowpass filter to accelerometer and gyro samples
        udb_xrate.value = lp2(udb_xrate.input, &gx, LPCB);
        udb_yrate.value = lp2(udb_yrate.input, &gy, LPCB);
        udb_zrate.value = lp2(udb_zrate.input, &gz, LPCB);
        udb_xaccel.value = lp2(udb_xaccel.input, &ax, LPCB);
        udb_yaccel.value = lp2(udb_yaccel.input, &ay, LPCB);
        udb_zaccel.value = lp2(udb_zaccel.input, &az, LPCB);
        primaryV.value = lp2(primaryV.input, &pv, LPCB);
        // shouldn't need to filter this reference voltage
        udb_vref.value = lp2(udb_vref.input, &rv, LPCB);

#define INTEGRATIONCOUNT 3
        // buffer raw accelerometer samples for logging: ADC rate is 20.833 KHz, scan rate 2.604 KHz
        // BUFFSIZE=5 samples, 4 integration periods per sample
        // log sample rate 2.6/3 = 868 Hz
        // log record rate = 868/BUFFSIZE = 173.6 Hz
        // accelBuffCount is cleared each time a telemetry record is sent

        // integrator
        if (accelBuffCount < BUFFSIZE) {
            acc_buffer[accPing][0][accelBuffCount] += (udb_xaccel.input >> 1) - (udb_xaccel.offset >> 1);
            acc_buffer[accPing][1][accelBuffCount] += (udb_yaccel.input >> 1) - (udb_yaccel.offset >> 1);
            acc_buffer[accPing][2][accelBuffCount] += (udb_zaccel.input >> 1) - (udb_zaccel.offset >> 1);
        }
        integrationCount++; // integrate and dump counter
        if (integrationCount >= INTEGRATIONCOUNT) {
            integrationCount = 0;

            // divide by N
            acc_buffer[accPing][0][accelBuffCount] = __builtin_divsd( acc_buffer[accPing][0][accelBuffCount], INTEGRATIONCOUNT );
            acc_buffer[accPing][1][accelBuffCount] = __builtin_divsd( acc_buffer[accPing][1][accelBuffCount], INTEGRATIONCOUNT );
            acc_buffer[accPing][2][accelBuffCount] = __builtin_divsd( acc_buffer[accPing][2][accelBuffCount], INTEGRATIONCOUNT );
            accelBuffCount++;

            if (accelBuffCount >= BUFFSIZE) {
                // about 20usec of cpu time required for the longest case of this ISR
                accelBuffCount = 0;
                // after BUFFSIZE iterations trigger telemetry
                // this sets the record rate
                callSendTelemetry = true;
                // switch to other buffer and dump it
                accPing = 1 - accPing;
                memset(&acc_buffer[accPing], 0, 3*BUFFSIZE*sizeof(int));
            }
        }
    }
    _LATD6 = 0;

    interrupt_restore_corcon;
    return;
}

#endif
