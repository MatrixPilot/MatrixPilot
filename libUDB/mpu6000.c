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

// Internal MPU6000 axis definition
// X axis pointing to right, Y axis pointing forward and Z axis pointing up

#include <stdio.h>
#include <spi.h>

#include "libUDB_internal.h"
#include "spiUtils.h"
#include "mpu6000.h"


//Sensor variables
unsigned int mpu_data[7], mpuCnt = 0;
bool mpuDAV = false;

#if BOARD_TYPE == AUAV2_BOARD
struct ADchannel udb_xaccel, udb_yaccel, udb_zaccel; // x, y, and z accelerometer channels
struct ADchannel udb_xrate, udb_yrate, udb_zrate; // x, y, and z gyro channels
struct ADchannel mpu_temp;
#endif

// MPU6000 Initialization and configuration
//FIXME: sometimes it is necessary to cycle power to init properly

void MPU6000_init16(void)
{
#if (BOARD_TYPE == AUAV2_BOARD)
    AD1PCFGLbits.PCFG2 = 1; // Configure SS1 pin as digital
#endif

    _TRISB2 = 0; // make SS1  an output

    // set prescaler for FCY/64 = 625KHz at 40MIPS
    initSPI1_master16(SEC_PRESCAL_4_1, PRI_PRESCAL_16_1);

    LED_RED = LED_ON;
    // need at least 6msec delay here
    __delay_ms(6);
    writeSPI1reg16(MPUREG_PWR_MGMT_1, BIT_H_RESET);
    LED_RED = LED_OFF;

    LED_YELLOW = LED_ON;
    // reset bit doesn't appear to ever be set
    //    int try = 0;
    //    uint8_t regVal = 0;
    //    while (!(regVal & BIT_H_RESET))
    //    {
    //        regVal = readSPI1reg16(MPUREG_PWR_MGMT_1);
    //        try++;
    //    }
    //    while (regVal)
    //    {
    //        regVal = readSPI1reg16(MPUREG_PWR_MGMT_1);
    //        try++;
    //    }
    // but a 1msec delay appears to be reliable
    __delay_ms(1);
    LED_YELLOW = LED_OFF;

    LED_GREEN = LED_ON;
    // Wake up device and select GyroZ clock (better performance)
    writeSPI1reg16(MPUREG_PWR_MGMT_1, MPU_CLK_SEL_PLLGYROZ);

    // Disable I2C bus (recommended on datasheet)
    writeSPI1reg16(MPUREG_USER_CTRL, BIT_I2C_IF_DIS);

    // SAMPLE RATE
    writeSPI1reg16(MPUREG_SMPLRT_DIV, 4); // Sample rate = 200Hz    Fsample= 1Khz/(4+1) = 200Hz

    // FS & DLPF   FS=2000º/s, DLPF = 42Hz (low pass filter)
    writeSPI1reg16(MPUREG_CONFIG, BITS_DLPF_CFG_42HZ); // BITS_DLPF_CFG_20HZ BITS_DLPF_CFG_42HZ BITS_DLPF_CFG_98HZ

    //	writeSPI1reg16(MPUREG_GYRO_CONFIG, BITS_FS_2000DPS);  // Gyro scale 2000º/s
    writeSPI1reg16(MPUREG_GYRO_CONFIG, BITS_FS_500DPS); // Gyro scale 500º/s

    //	writeSPI1reg16(MPUREG_ACCEL_CONFIG, BITS_FS_2G);           // Accel scele 2g
    writeSPI1reg16(MPUREG_ACCEL_CONFIG, BITS_FS_8G); // Accel scale g = 4096

    // INT CFG => Interrupt on Data Ready
    writeSPI1reg16(MPUREG_INT_PIN_CFG, BIT_INT_LEVEL | BIT_INT_RD_CLEAR); // INT: Clear on any read
    //    writeSPI1reg16(MPUREG_INT_PIN_CFG, BIT_INT_LEVEL | BIT_LATCH_INT_EN | BIT_INT_RD_CLEAR);
    //    writeSPI1reg16(MPUREG_INT_PIN_CFG, 0x1); // INT: clock out, pulse, no clear

    writeSPI1reg16(MPUREG_INT_ENABLE, BIT_DATA_RDY_EN); // INT: Raw data ready

#if (BOARD_TYPE == UDB4_BOARD)
    // set prescaler for FCY/4 = 10MHz at 40MIPS
    initSPI1_master16(SEC_PRESCAL_4_1, PRI_PRESCAL_1_1);

    AD1PCFGHbits.PCFG20 = 1; // Configure INT1 pin as digital
    TRISAbits.TRISA12 = 1; // make INT1 an input
#elif (BOARD_TYPE == AUAV2_BOARD)
    // set prescaler for FCY/4 = 8MHz at 40MIPS
    initSPI1_master16(SEC_PRESCAL_5_1, PRI_PRESCAL_1_1);

    _TRISE8 = 1; // make INT1 an input
#endif
    
    INTCON2bits.INT1EP = 1; // Setup INT1 pin to interrupt on falling edge
    IFS1bits.INT1IF = 0; // Reset INT1 interrupt flag
    IEC1bits.INT1IE = 1; // Enable INT1 Interrupt Service Routine
    _INT1IP = 6;

    //    AD1PCFGHbits.PCFG21 = 1;    // Configure INT2 pin as digital
    //    TRISAbits.TRISA13 = 1; // make INT2 an input
    //    _INT2EP = 1;
    //    _INT2IF = 0;
    //    _INT2IE = 1;
    //    _INT2IP = 6;
}

void MPU6000_read(void)
{
    // this is working
    //    d1 = readSPI1reg16(MPUREG_INT_PIN_CFG);
    //        delay_us(10); // without delay

    // burst read guarantees that all registers represent the same sample interval
    mpuCnt++;
    readSPI1_burst16n(mpu_data, 7, MPUREG_ACCEL_XOUT_H);
}

void __attribute__((interrupt, no_auto_psv)) _INT1Interrupt(void)
{
    indicate_loading_inter;
    interrupt_save_set_corcon;
    _INT1IF = 0; // Clear the INT1 interrupt flag


    //    LED_BLUE = LED_ON;
    MPU6000_read();
    mpuDAV = true;
    //    LED_BLUE = LED_OFF;

#if (BOARD_TYPE == AUAV2_BOARD)
    // this board has only the MPU-6000
    // filtering is done onboard the MPU-6000, so input field is unused
    udb_xaccel.value = mpu_data[0];
    udb_yaccel.value = mpu_data[1];
    udb_zaccel.value = mpu_data[2];

    mpu_temp.value = mpu_data[3];

    udb_xrate.value = mpu_data[4];
    udb_yrate.value = mpu_data[5];
    udb_zrate.value = mpu_data[6];

    // execute heartbeat code
    doT1Interrupt();
#endif

    interrupt_restore_corcon;
}

void MPU6000_print(void)
{
    printf("%06u axyz %06i %06i %06i gxyz %06i %06i %06i t %u\r\n",
           mpuCnt, mpu_data[0], mpu_data[1], mpu_data[2], mpu_data[4], mpu_data[5], mpu_data[6], mpu_data[3]);
}

#if 0
// MPU6000 Initialization and configuration
//FIXME: sometimes it is necessary to cycle power to init properly

void MPU6000_init(void)
{
    initSPI1_master(SEC_PRESCAL_4_1, PRI_PRESCAL_16_1);

    unsigned int wdata;
    int i;
    wdata = readSPI1reg16(MPUREG_PWR_MGMT_1);
    i = 1;

    LED_RED = LED_ON;
    // need at least 6msec delay here
    __delay_ms(6);
    writeSPI1reg(MPUREG_PWR_MGMT_1, BIT_H_RESET);
    LED_RED = LED_OFF;

    LED_YELLOW = LED_ON;
    // reset bit doesn't appear to ever be set
    //    int try = 0;
    //    uint8_t regVal = 0;
    //    while (regVal & BIT_H_RESET)
    //    {
    //        regVal = readSPI1reg(MPUREG_PWR_MGMT_1);
    //        try++;
    //    }
    // but a 1msec delay appears to be reliable
    __delay_ms(1);
    LED_YELLOW = LED_OFF;

    LED_GREEN = LED_ON;
    // Wake up device and select GyroZ clock (better performance)
    writeSPI1reg(MPUREG_PWR_MGMT_1, MPU_CLK_SEL_PLLGYROZ);
    // Disable I2C bus (recommended on datasheet)
    writeSPI1reg(MPUREG_USER_CTRL, BIT_I2C_IF_DIS);
    // SAMPLE RATE
    writeSPI1reg(MPUREG_SMPLRT_DIV, 0x04); // Sample rate = 200Hz    Fsample= 1Khz/(4+1) = 200Hz
    // FS & DLPF   FS=2000º/s, DLPF = 42Hz (low pass filter)
    writeSPI1reg(MPUREG_CONFIG, BITS_DLPF_CFG_42HZ); // BITS_DLPF_CFG_20HZ BITS_DLPF_CFG_42HZ BITS_DLPF_CFG_98HZ
    //	writeSPI1reg(MPUREG_GYRO_CONFIG, BITS_FS_2000DPS);  // Gyro scale 2000º/s
    writeSPI1reg(MPUREG_GYRO_CONFIG, BITS_FS_500DPS); // Gyro scale 500º/s
    //	writeSPI1reg(MPUREG_ACCEL_CONFIG, BITS_FS_2G);           // Accel scele 2g (g=8192)
    writeSPI1reg(MPUREG_ACCEL_CONFIG, BITS_FS_8G); // Accel scele 8g

    //    writeSPI1reg(0x23, 0xF8); // write most registers to fifo

    // INT CFG => Interrupt on Data Ready
    writeSPI1reg(MPUREG_INT_ENABLE, BIT_DATA_RDY_EN); // INT: Raw data ready
    //    writeSPI1reg(MPUREG_INT_PIN_CFG, BIT_INT_LEVEL | BIT_INT_RD_CLEAR); // INT: Clear on any read
    //    writeSPI1reg(MPUREG_INT_PIN_CFG, BIT_LATCH_INT_EN | BIT_CLKOUT_EN); // INT: Clear on any read
    writeSPI1reg(MPUREG_INT_PIN_CFG, 0x1); // INT: clock out, pulse, no clear

    wdata = readSPI1reg16(MPUREG_PWR_MGMT_1);
    i = 3;

    wdata = readSPI1reg16(MPUREG_INT_ENABLE);
    i = 3;

    wdata = readSPI1reg16(MPUREG_INT_PIN_CFG);
    i = 3;

    initSPI1_master(SEC_PRESCAL_4_1, PRI_PRESCAL_4_1);

    //#define INT1        PORTAbits.RA12
    //#define INT1_TRIS   TRISAbits.TRISA12
    TRISAbits.TRISA12 = 1; // make INT1 an input
    INTCON2bits.INT1EP = 1; // Setup INT1 pin to interrupt on falling edge
    IFS1bits.INT1IF = 0; // Reset INT1 interrupt flag
    IEC1bits.INT1IE = 1; // Enable INT1 Interrupt Service Routine
}
#endif
