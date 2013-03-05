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
#include "a2d_dma_udb4.h"

#if (BOARD_TYPE == UDB4_BOARD)

//	Analog to digital processing.
//	Sampling and conversion is done automatically, so that all that needs to be done during
//	interrupt processing is to read the data out of the buffer.
//	Raw samples are taken approximately 500 per second per channel.
//	A first order digital lowpass filter with a time constant of about 32 milliseconds
//  is applied to improve signal to noise.

//	Variables.

extern boolean callSendTelemetry;

int accb_cnt = 0;
long acc_accum[2][3][BUFFSIZE];
int acc_buff[2][3][BUFFSIZE];
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
// *** from 31 to 16=> ADC_RATE = 23.148 KHz
#define ADSAMP_TIME_N 13
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
#define NUM_AD_CHAN 8

// Number of locations for ADC buffer = 8 words
// Align the buffer to 10 words or 20 bytes. This is needed for peripheral indirect mode
int BufferA[NUM_AD_CHAN] __attribute__((space(dma), aligned(32)));
int BufferB[NUM_AD_CHAN] __attribute__((space(dma), aligned(32)));

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

    AD1CON1bits.FORM = 3; // signed fractional
    AD1CON1bits.SSRC = 7; // auto convert
    AD1CON1bits.ASAM = 1; // auto samp
    AD1CON1bits.AD12B = 1; // 12 bit A to D

    AD1CON2bits.CSCNA = 1; // scanA ch0
    AD1CON2bits.CHPS = 0; // Converts CH0

    AD1CON3bits.ADRC = 0; // ADC Clock is derived from Systems Clock
    AD1CON3bits.ADCS = ADCLK_DIV_N_MINUS_1; // TAD = (15+1)/(FREQOSC/2) = 0.4usec at 40mips

    AD1CON3bits.SAMC = ADSAMP_TIME_N;

    AD1CON2bits.VCFG = 0; // use supply as reference voltage

    AD1CON1bits.ADDMABM = 1; // DMA buffers are built in sequential mode
    // N_CHANNELS_SCANNED samples ??? this affects both DMA and non-DMA operation
    AD1CON2bits.SMPI = NUM_AD_CHAN - 1;
    AD1CON4bits.DMABL = 0; // Each buffer contains 1 word

    AD1CSSL = 0x0000;
    AD1CSSH = 0x0000;
    AD1PCFGL = 0xFFFF;
    AD1PCFGH = 0xFFFF;

    //	include the 110 degree/second scale, gyro
    /*
    AD1CSSLbits.CSS0 = 1 ;
    AD1CSSLbits.CSS3 = 1 ;
    AD1CSSLbits.CSS5 = 1 ;

    AD1PCFGLbits.PCFG0 = 0 ;
    AD1PCFGLbits.PCFG3 = 0 ;
    AD1PCFGLbits.PCFG5 = 0 ;
     */

    //	include the 500 degree/second scale, gyro
    AD1CSSLbits.CSS1 = 1;
    AD1CSSLbits.CSS4 = 1;
    AD1CSSLbits.CSS6 = 1;

    AD1PCFGLbits.PCFG1 = 0;
    AD1PCFGLbits.PCFG4 = 0;
    AD1PCFGLbits.PCFG6 = 0;

    //	include the accelerometer in the scan:
    AD1CSSLbits.CSS9 = 1;
    AD1CSSLbits.CSS10 = 1;
    AD1CSSLbits.CSS11 = 1;

    AD1PCFGLbits.PCFG9 = 0;
    AD1PCFGLbits.PCFG10 = 0;
    AD1PCFGLbits.PCFG11 = 0;

    //      include AN12,15 in the scan
    // WARNING: ADC2 can access only inputs AN0-AN15, AN16-31 are not available
    // This means the PCB pads AN16-18 are unusable with this version of code
    // Simply changing "AD1" to "AD1" will fix the problem
    AD1CSSLbits.CSS12 = 1;
    AD1CSSLbits.CSS15 = 1;
    AD1PCFGLbits.PCFG12 = 0;
    AD1PCFGLbits.PCFG15 = 0;

    _AD1IF = 0; // clear the AD interrupt
    _AD1IP = 5; // priority 5
    _AD1IE = 0; // Do Not Enable A/D interrupt
    AD1CON1bits.ADON = 1; // turn on the A to D

    //  DMA Setup
    DMA0CONbits.AMODE = 2; // Configure DMA for Peripheral indirect mode
    DMA0CONbits.MODE = 2; // Configure DMA for Continuous Ping-Pong mode
    DMA0PAD = (int) &ADC1BUF0;
    DMA0CNT = NUM_AD_CHAN - 1;
    DMA0REQ = 13; // Select ADC1 as DMA Request source

    DMA0STA = __builtin_dmaoffset(BufferA);
    DMA0STB = __builtin_dmaoffset(BufferB);

    IFS0bits.DMA0IF = 0; //Clear the DMA interrupt flag bit
    IEC0bits.DMA0IE = 1; //Set the DMA interrupt enable bit

    DMA0CONbits.CHEN = 1; // Enable DMA

    return;
}

// 3dB frequency about 45Hz at 2KHz sample rate, 38Hz at fs = 1.7 KHz
#define LPCB (unsigned int)(65536 / 12)


unsigned char DmaBuffer = 0;

void __attribute__((__interrupt__, __no_auto_psv__)) _DMA0Interrupt(void) {
    static union int32_w2 gx, gy, gz;
    static union int32_w2 ax, ay, az;
    static union int32_w2 pv, rv;

    indicate_loading_inter;
    interrupt_save_set_corcon;

    IFS0bits.DMA0IF = 0; // Clear the DMA0 Interrupt Flag
    DmaBuffer ^= 1; // Switch buffers

#if (RECORD_FREE_STACK_SPACE == 1)
    unsigned int stack = WREG15;
    if (stack > maxstack) {
        maxstack = stack;
    }
#endif

#if (HILSIM != 1)
    int *CurBuffer = (DmaBuffer == 0) ? BufferA : BufferB;

    udb_xrate.input = CurBuffer[xrateBUFF - 1];
    udb_yrate.input = CurBuffer[yrateBUFF - 1];
    udb_zrate.input = CurBuffer[zrateBUFF - 1];

//    udb_xaccel.input = 1000;
//    udb_yaccel.input = -2000;
//    udb_zaccel.input = 1000;

    udb_xaccel.input = CurBuffer[xaccelBUFF - 1];
    udb_yaccel.input = CurBuffer[yaccelBUFF - 1];
    udb_zaccel.input = CurBuffer[zaccelBUFF - 1];

    udb_vref.input = CurBuffer[analogInput1BUFF - 1];
    primaryV.input = CurBuffer[analogInput2BUFF - 1];

#endif

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

#if (TELEMETRY_TYPE == 9)
    static int intCtr = 0;
#define INTEGRATIONCOUNT 2
    // buffer raw accelerometer samples for logging: ADC rate is 23.148 KHz, scan rate 2.893 KHz
    // BUFFSIZE=5 samples, 2 integration periods per sample
    // log sample rate 2.893/2 = 1447 Hz
    // log record rate = 1447/BUFFSIZE = 289 Hz
    // accelBuffCount is cleared each time a telemetry record is sent

    // integrator
    if (accb_cnt < BUFFSIZE) {
        acc_accum[accPing][0][accb_cnt] += udb_xaccel.input;
        acc_accum[accPing][1][accb_cnt] += udb_yaccel.input;
        acc_accum[accPing][2][accb_cnt] += udb_zaccel.input;
    }
    intCtr++; // integrate and dump counter
    if (intCtr >= INTEGRATIONCOUNT) {
        intCtr = 0;

        // divide by N
        acc_buff[accPing][0][accb_cnt] = __builtin_divsd(acc_accum[accPing][0][accb_cnt], INTEGRATIONCOUNT);
        acc_buff[accPing][1][accb_cnt] = __builtin_divsd(acc_accum[accPing][1][accb_cnt], INTEGRATIONCOUNT);
        acc_buff[accPing][2][accb_cnt] = __builtin_divsd(acc_accum[accPing][2][accb_cnt], INTEGRATIONCOUNT);
        accb_cnt++;

        if (accb_cnt >= BUFFSIZE) {
            // about 20usec of cpu time required for the longest case of this ISR
            accb_cnt = 0;
//            _LATD6 = 1;
            // switch to other buffer and dump it
            accPing = 1 - accPing;
            memset(&acc_accum[accPing], 0, 3 * BUFFSIZE * sizeof (long));

            // after BUFFSIZE iterations trigger telemetry to send previous buffer
            // this sets the record rate
            callSendTelemetry = true;
        }
    }

//    _LATD6 = 0;
#endif

    interrupt_restore_corcon;
    return;
}

#endif
