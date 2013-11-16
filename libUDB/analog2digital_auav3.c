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
#include "oscillator.h"
#include "interrupt.h"
#include "heartbeat.h"

#if (BOARD_TYPE == AUAV3_BOARD)

//#define ADC_HZ 500000
//#define ALMOST_ENOUGH_SAMPLES 110 // there are ? samples in a sum
//
//#if (((FCY / ADC_HZ) - 1) > 255)
//#error Invalid ADC_HZ configuration
//#endif
#if (NUM_ANALOG_INPUTS <= 0)
#undef NUM_ANALOG_INPUTS
#define NUM_ANALOG_INPUTS                   4
#endif

//	Variables.
#if (NUM_ANALOG_INPUTS >= 1)
struct ADchannel udb_analogInputs[NUM_ANALOG_INPUTS]; // 0-indexed, unlike servo pwIn/Out/Trim arrays
#endif
struct ADchannel udb_vcc;
struct ADchannel udb_5v;
struct ADchannel udb_rssi;
struct ADchannel udb_vref; // reference voltage (deprecated, here for MAVLink compatibility)

// Align the buffer. This is needed for peripheral indirect mode
#define NUM_AD_CHAN 7
__eds__ int16_t BufferA[NUM_AD_CHAN] __attribute__((eds,space(dma),aligned(32)));
__eds__ int16_t BufferB[NUM_AD_CHAN] __attribute__((eds,space(dma),aligned(32)));

//int16_t vref_adj;
int16_t sample_count;
uint8_t DmaBuffer = 0;

#if (RECORD_FREE_STACK_SPACE == 1)
uint16_t maxstack = 0;
#endif

// dsPIC33FJXXXGPX06A/X08A/X10A
// minimum allowed 12-bit ADC clock period is 118ns or 8.47MHz
// minimum allowed 10-bit ADC clock period is 76ns, or 13.16MHz
// 12 bit mode conversion time is 14 TAD cycles
// total sample/conversion time is (14+SAMC) * TAD
// for 400nsec TAD, total is 10usec for 100KHz conversion rate with SAMC = 11
// *** observed 72usec interval between interrupts on scope => interrupt every sample

// desired adc clock is 1.1MHz and conversion rate is 25KHz
// (1.1MHz is the lowest rate achievable at FCY = 70MHz
//#define DES_ADC_CLK (1100000LL)
#define DES_ADC_CLK (FCY / (63 + 1))
#define DES_ADC_RATE (25000LL)

// calculate adc clock prescaler setting
#define ADCLK_DIV_N_MINUS_1 ((FCY / DES_ADC_CLK) - 1)
#if (ADCLK_DIV_N_MINUS_1 > 63)
#error "FCY too high to achieve desired ADC clock rate"
#endif

// calculate setting for desired sampling interval
#define ADSAMP_TIME_N (((FCY / (ADCLK_DIV_N_MINUS_1 + 1)) / DES_ADC_RATE) - 14)
#if (ADSAMP_TIME_N > 31)
#error "ADC clock rate too high to achieve desired ADC sample rate"
#endif

// TAD is 1/ADC_CLK
#define ADC_CLK (FCY / (ADCLK_DIV_N_MINUS_1 + 1))
const uint32_t adc_clk = ADC_CLK;

#define ADC_RATE (ADC_CLK / (ADSAMP_TIME_N + 14))
const uint32_t adc_rate = ADC_RATE;

//#define ALMOST_ENOUGH_SAMPLES ((ADC_RATE / (NUM_AD_CHAN * HEARTBEAT_HZ)) - 2)
#define ALMOST_ENOUGH_SAMPLES 20
const uint32_t almost_enough = ALMOST_ENOUGH_SAMPLES;

//#define _SELECTED_VALUE(l,v) #l#v
//#define SELECTED_VALUE(macro) _SELECTED_VALUE(#macro,macro)
//#pragma message (SELECTED_VALUE(ADCLK_DIV_N_MINUS_1))
//#pragma message (SELECTED_VALUE(ADC_CLK))
//#pragma message (SELECTED_VALUE(ADC_RATE))
//#pragma message (SELECTED_VALUE(ALMOST_ENOUGH_SAMPLES))

void udb_init_ADC(void)
{
	DPRINT("ADCLK_DIV_N_MINUS_1 = %li\r\n", (int32_t)ADCLK_DIV_N_MINUS_1);
	DPRINT("ADC_CLK = %li\r\n", (int32_t)ADC_CLK);
	DPRINT("ADC_RATE = %li\r\n", (int32_t)ADC_RATE);
	DPRINT("ADSAMP_TIME_N = %li\r\n", (int32_t)ADSAMP_TIME_N);
	DPRINT("ALMOST_ENOUGH_SAMPLES = %li\r\n", (int32_t)ALMOST_ENOUGH_SAMPLES);
/*
MatrixPilot 10:37:23 Aug 17 2013 @ 16 mips
ADCLK_DIV_N_MINUS_1 = 13
ADC_CLK = 1142857
ADC_RATE = 25396
ADSAMP_TIME_N = 31
ALMOST_ENOUGH_SAMPLES = 88
 */
	sample_count = 0;

	AD1CON1bits.FORM  = 3;      // Data Output Format: Signed Fraction (Q15 format)
//	AD1CON1bits.FORM  = 0;      // Data Output Format: Integer
	AD1CON1bits.SSRC  = 7;      // Sample Clock Source: Auto-conversion
	AD1CON1bits.ASAM  = 1;      // ADC Sample Control: Sampling begins immediately after conversion
	AD1CON1bits.AD12B = 1;      // 12-bit ADC operation
	AD1CON2bits.CSCNA = 1;      // Scan Input Selections for CH0+ during Sample A bit
	AD1CON2bits.CHPS  = 0;      // Converts CH0
	AD1CON3bits.ADRC  = 0;      // ADC Clock is derived from System Clock

	AD1CON3bits.ADCS = ADCLK_DIV_N_MINUS_1; // TAD = (15+1)/FCY = 0.4usec at 40mips
//	AD1CON3bits.ADCS    = ((FCY / ADC_HZ) - 1);
//	AD1CON3bits.ADCS    = 11;   // ADC Conversion Clock Tad=Tcy*(ADCS+1)= (1/40M)*12 = 0.3us (3333.3Khz)
//	                            // ADC Conversion Time for 12-bit Tc=14*Tad = 4.2us
	AD1CON3bits.SAMC = ADSAMP_TIME_N;
//	AD1CON3bits.SAMC    = 1;    // No waiting between samples
	AD1CON2bits.VCFG    = 0;    // use supply as reference voltage
	AD1CON1bits.ADDMABM = 1;    // DMA buffers are built in sequential mode
	AD1CON2bits.SMPI    = (NUM_AD_CHAN-1);
	AD1CON4bits.DMABL   = 0;    // Each buffer contains 1 word
	AD1CON4bits.ADDMAEN = 1;    // use DMA instead of FIFO

	AD1CSSL = 0x0000;
	AD1CSSH = 0x0000;

	// power-on default is all analog inputs selected
	ANSELA = 0;                 // disable all analog inputs on port A
	ANSELC = 0;                 // disable all analog inputs on port C
	ANSELD = 0;                 // disable all analog inputs on port D
	ANSELE = 0;                 // disable all analog inputs on port E
	ANSELG = 0;                 // disable all analog inputs on port G

	// enable specific analog inputs on port B
	// AN6:9,13:15 map to:
	// AUAV3 inputs ANA2:3, ANA0:1, V, I, RS
	uint16_t mask = ((1 << 6) | (1 << 7) | (1 << 8) | (1 << 9) | (1 << 13) | (1 << 14) | (1 << 15));
	ANSELB = mask;

	// set analog pins as inputs
	TRISB |= mask;

	// include voltage monitor inputs
	_CSS13 = 1;                 // Enable AN13 for channel scan
	_CSS14 = 1;                 // Enable AN14 for channel scan
	_CSS15 = 1;                 // Enable AN15 for channel scan

	// include the extra analog input pins
	_CSS6 = 1;                  // Enable AN6 for channel scan
	_CSS7 = 1;                  // Enable AN7 for channel scan
	_CSS8 = 1;                  // Enable AN8 for channel scan
	_CSS9 = 1;                  // Enable AN9 for channel scan

	_AD1IF = 0;                 // Clear the A/D interrupt flag bit
	_AD1IP = INT_PRI_AD1;       // Set the interrupt priority
	_AD1IE = 0;                 // Do Not Enable A/D interrupt
	AD1CON1bits.ADON = 1;       // Turn on the A/D converter

	// DMA Setup
	DMA0CONbits.AMODE = 2;      // Configure DMA for Peripheral indirect mode
	DMA0CONbits.MODE = 2;       // Configure DMA for Continuous Ping-Pong mode
	DMA0PAD = (uint16_t)&ADC1BUF0;
	DMA0CNT = NUM_AD_CHAN-1;
	DMA0REQ = 13;               // Select ADC1 as DMA Request source

	DMA0STAH = 0x0000;
	DMA0STAL = __builtin_dmaoffset(BufferA);
	DMA0STBH = 0x0000;
	DMA0STBL = __builtin_dmaoffset(BufferB);

	_DMA0IP = INT_PRI_DMA0;     // Set the DMA ISR priority
	IFS0bits.DMA0IF = 0;        // Clear the DMA interrupt flag bit
	IEC0bits.DMA0IE = 1;        // Set the DMA interrupt enable bit

	DMA0CONbits.CHEN = 1;       // Enable DMA
}

void __attribute__((__interrupt__, __no_auto_psv__)) _DMA0Interrupt(void)
{
	DIG2 = 1;
	indicate_loading_inter;
	interrupt_save_set_corcon;


#if (RECORD_FREE_STACK_SPACE == 1)
	uint16_t stack = SP_current();
	if (stack > maxstack)
	{
		maxstack = stack;
	}
#endif

	int i;

#if (HILSIM != 1)
	__eds__ int16_t *CurBuffer = (DmaBuffer == 0) ? BufferA : BufferB;
	for (i = 0; i < NUM_ANALOG_INPUTS; i++)
	{
		udb_analogInputs[i].input = CurBuffer[i];
	}
#endif // HILSIM

	DmaBuffer ^= 1;             // Switch buffers
#if (BOARD_TYPE == AUAV3_BOARD)
	_DMA0IF = 0;                // Clear the DMA Interrupt Flag
#endif // (BOARD_TYPE == AUAV3_BOARD)

	if (udb_flags._.a2d_read == 1) // prepare for the next reading
	{
		udb_flags._.a2d_read = 0;
		for (i = 0; i < NUM_ANALOG_INPUTS; i++)
		{
			udb_analogInputs[i].sum = 0;
		}
//
//		static int i = 0;
//		if (i++ > HEARTBEAT_HZ) {
//			i = 0;
//			printf("sc %u\r\n", sample_count);
//		}
//
		sample_count = 0;
	}

	// perform the integration:
	for (i = 0; i < NUM_ANALOG_INPUTS; i++)
	{
		udb_analogInputs[i].sum += udb_analogInputs[i].input;
	}
	sample_count++;

	// When there is a chance that data will be read soon,
	// have the new average values ready.
	if (sample_count > ALMOST_ENOUGH_SAMPLES)
	{
		for (i = 0; i < NUM_ANALOG_INPUTS; i++)
		{
			udb_analogInputs[i].value = __builtin_divsd(udb_analogInputs[i].sum, sample_count);
		}
	}
	interrupt_restore_corcon;
	DIG2 = 0;
}

#endif // BOARD_TYPE
