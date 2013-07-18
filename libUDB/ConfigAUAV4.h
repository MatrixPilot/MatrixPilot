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


// used for the AUAV4 with PIC32MX

//typedef char int8_t;
//typedef int  int16_t;
//typedef long int32_t;

//typedef unsigned char uint8_t;
//typedef unsigned int  uint16_t;
//typedef unsigned long uint32_t;


extern int pic32_corcon;

//#define fractional int

#define _T1IP IPC1bits.T1IP
#define _T1IF IFS0bits.T1IF
#define _T1IE IEC0bits.T1IE

#define _T4IP IPC4bits.T4IP
#define _T4IF IFS0bits.T4IF
#define _T4IE IEC0bits.T4IE

#define _T5IP IPC5bits.T5IP
#define _T5IF IFS0bits.T5IF
#define _T5IE IEC0bits.T5IE

#define _T6IP IPC2bits.T2IP
#define _T6IF IFS0bits.T2IF
#define _T6IE IEC0bits.T2IE

#define _T7IP IPC3bits.T3IP
#define _T7IF IFS0bits.T3IF
#define _T7IE IEC0bits.T3IE

#define _C1IP PR1
#define _C1IF IFS0bits.T1IF
#define _C1IE IEC0bits.T1IE

#define _C2IP PR1
#define _C2IF IFS0bits.T1IF
#define _C2IE IEC0bits.T1IE

#define _IC1IP IPC1bits.IC1IP
#define _IC1IF IFS0bits.IC1IF
#define _IC1IE IEC0bits.IC1IE

#define _U1RXIP IPC6bits.U1IP
#define _U1RXIF IFS0bits.U1RXIF
#define _U1RXIE IEC0bits.U1RXIE

#define _U2RXIP IPC8bits.U2IP
#define _U2RXIF IFS1bits.U2RXIF
#define _U2RXIE IEC1bits.U2RXIE

#define _U1TXIP IPC6bits.U1IP
#define _U1TXIF IFS0bits.U1TXIF
#define _U1TXIE IEC0bits.U1TXIE

#define _U2TXIP IPC8bits.U2IP
#define _U2TXIF IFS1bits.U2TXIF
#define _U2TXIE IEC1bits.U2TXIE

#define _MI2C1IP IPC6bits.I2C1IP
#define _MI2C1IF IFS0bits.I2C1MIF
#define _MI2C1IE IEC0bits.I2C1MIE

#define _MI2C2IP IPC8bits.I2C2IP
#define _MI2C2IF IFS1bits.I2C2MIF
#define _MI2C2IE IEC1bits.I2C2MIE

#define SEC_PRESCAL_1_1 1
#define PRI_PRESCAL_1_1 1
#define SEC_PRESCAL_4_1 1
#define PRI_PRESCAL_16_1 1
#define SEC_PRESCAL_2_1 1

#define _INT1EP INTCONbits.INT1EP
#define _INT1IP IPC1bits.INT1IP
#define _INT1IF IFS0bits.INT1IF
#define _INT1IE IEC0bits.INT1IE


#define SPI_INT_DIS 1
#define SPI_INT_PRI_6 1
#define ENABLE_SDO_PIN 1
#define SPI_MODE16_ON 1
#define ENABLE_SCK_PIN 1
#define SPI_SMP_ON 1
#define SPI_CKE_OFF 1
#define SLAVE_ENABLE_OFF 1
#define CLK_POL_ACTIVE_LOW 1
#define MASTER_ENABLE_ON 1
#define FRAME_ENABLE_OFF 1
#define FRAME_SYNC_OUTPUT 1
#define SPI_ENABLE 1
#define SPI_IDLE_CON 1
#define SPI_RX_OVFLOW_CLR 1

#define _SPI1IP IPC5bits.SPI1IP
#define _SPI1IF IFS0bits.SPI1TXIF
#define _SPI1IE IEC0bits.SPI1TXIE

#define _SPI2IP IPC7bits.SPI2IP
#define _SPI2IF IFS1bits.SPI2TXIF
#define _SPI2IE IEC1bits.SPI2TXIE


#define _SWR pic32_corcon
#define CORCON pic32_corcon


#define __eds__

// Fake ASM calls
#define __builtin_mulss(x,y) ((( int32_t)(x))*( int32_t)(y))
#define __builtin_mulus(x,y) (((uint32_t)(x))*( int32_t)(y))
#define __builtin_mulsu(x,y) ((( int32_t)(x))*(uint32_t)(y))
#define __builtin_muluu(x,y) (((uint32_t)(x))*(uint32_t)(y))

#define __builtin_divud(x,y) ((uint16_t)(((uint32_t)(x))/(uint16_t)(y)))
#define __builtin_divsd(x,y) (( int16_t)((( int32_t)(x))/(int16_t)(y)))


#define ACCEL_RANGE         4       //    4 g range

// note : it is possible to use other accelerometer ranges on the MPU6000
#define SCALEGYRO           3.0016  // 500 degree/second range
#define SCALEACCEL          1.29    // 4 g range

// A/D channels:
#define A_VCC_BUFF          5       // V, pin label Bat Volt
#define A_5V_BUFF           6       // I, pin label CS Curr
#define A_RSSI_BUFF         7       // RS, pin label RSSI
#define analogInput1BUFF    3
#define analogInput2BUFF    4
#define analogInput3BUFF    1
#define analogInput4BUFF    2

// MPU6000 configuration
//FIXME: these settings are for UDB4+breakout board

#define xrate_MPU_channel   5
#define yrate_MPU_channel   4
#define zrate_MPU_channel   6
#define temp_MPU_channel    3
#define xaccel_MPU_channel  1
#define yaccel_MPU_channel  0
#define zaccel_MPU_channel  2

#define XRATE_SIGN          -
#define YRATE_SIGN          -
#define ZRATE_SIGN          -
#define XACCEL_SIGN         +
#define YACCEL_SIGN         +
#define ZACCEL_SIGN         +

// Max inputs and outputs
#define MAX_INPUTS          8
#define MAX_OUTPUTS         8

// LED pins
#define LED_BLUE            LATEbits.LATE2
#define LED_RED             LATEbits.LATE1
#define LED_GREEN           LATEbits.LATE3
#define LED_ORANGE          LATEbits.LATE0
#define DIG2                LATEbits.LATE7
#define DIG1                LATEbits.LATE6
#define DIG0                LATEbits.LATE5

// SPI SS pin definitions
#define SPI1_SS             _LATE4
#define SPI2_SS             _LATE7
#define SPI1_TRIS           _TRISE4
#define SPI2_TRIS           _TRISE7

// Input Capture module input pins: PINX is the port pin, RPINX is used for PPS
// PINX must match RPINX for radionIn to work properly
#define IC_PIN1  _RD0
#define IC_RPIN1 64
#define IC_PIN2  _RD8
#define IC_RPIN2 72
#define IC_PIN3  _RD11
#define IC_RPIN3 75
#define IC_PIN4  _RA15
#define IC_RPIN4 31
#define IC_PIN5  _RA5
#define IC_RPIN5 21
#define IC_PIN6  _RA14
#define IC_RPIN6 30
#define IC_PIN7  _RA4
#define IC_RPIN7 20
#define IC_PIN8  _RF8
#define IC_RPIN8 104

