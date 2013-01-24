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
#include "../libDCM/libDCM_internal.h"

//Sensor variables
//unsigned int mpu_data[7], mpuCnt = 0;
unsigned int mpu_data[8], mpuCnt = 0;
bool mpuDAV = false;

struct ADchannel udb_xaccel, udb_yaccel, udb_zaccel; // x, y, and z accelerometer channels
struct ADchannel udb_xrate, udb_yrate, udb_zrate; // x, y, and z gyro channels
struct ADchannel mpu_temp;
int vref_adj;

// MPU6000 Initialization and configuration

void MPU6000_init16(void) {

    MPUSPI_SS = 1;    // deassert MPU SS
    MPUSPI_TRIS = 0; // make MPU SS  an output

    // set prescaler for FCY/64 = 625KHz at 40MIPS
    initMPUSPI_master16(SEC_PRESCAL_4_1, PRI_PRESCAL_16_1);

    //        LED_RED = LED_ON;
    // need at least 60 msec delay here
    __delay_ms(60);
    writeMPUSPIreg16(MPUREG_PWR_MGMT_1, BIT_H_RESET);
    //        LED_RED = LED_OFF;

    //        LED_GREEN = LED_ON;
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
    //    LED_YELLOW = LED_OFF;

    //        LED_GREEN = LED_ON;
    // Wake up device and select GyroZ clock (better performance)
    writeMPUSPIreg16(MPUREG_PWR_MGMT_1, MPU_CLK_SEL_PLLGYROZ);

    // Disable I2C bus (recommended on datasheet)
    writeMPUSPIreg16(MPUREG_USER_CTRL, BIT_I2C_IF_DIS);
#define USE_MPU 1
#if (USE_MPU == 1)
    // SAMPLE RATE
    writeMPUSPIreg16(MPUREG_SMPLRT_DIV, 4); // Sample rate = 200Hz    Fsample= 1Khz/(N+1) = 200Hz

    // scaling & DLPF
    writeMPUSPIreg16(MPUREG_CONFIG, BITS_DLPF_CFG_42HZ);

    //	writeMPUSPIreg16(MPUREG_GYRO_CONFIG, BITS_FS_2000DPS);  // Gyro scale 2000º/s
    writeMPUSPIreg16(MPUREG_GYRO_CONFIG, BITS_FS_500DPS); // Gyro scale 500º/s

#if ACCEL_RANGE == 2
    writeMPUSPIreg16(MPUREG_ACCEL_CONFIG, BITS_FS_2G); // Accel scele 2g, g = 8192
#elif ACCEL_RANGE == 4
    writeMPUSPIreg16(MPUREG_ACCEL_CONFIG, BITS_FS_4G); // Accel scale g = 4096
#elif ACCEL_RANGE == 8
    writeMPUSPIreg16(MPUREG_ACCEL_CONFIG, BITS_FS_8G); // Accel scale g = 2048
#else
#error "Invalid ACCEL_RANGE"
#endif

#else
    // SAMPLE RATE
    writeMPUSPIreg16(MPUREG_SMPLRT_DIV, 7); // Sample rate = 1KHz    Fsample= 8Khz/(N+1)

    // no DLPF, gyro sample rate 8KHz
    writeMPUSPIreg16(MPUREG_CONFIG, BITS_DLPF_CFG_256HZ_NOLPF2);

    writeMPUSPIreg16(MPUREG_GYRO_CONFIG, BITS_FS_500DPS); // Gyro scale 500º/s

    //        writeMPUSPIreg16(MPUREG_ACCEL_CONFIG, BITS_FS_2G); // Accel scele 2g, g = 16384
    writeMPUSPIreg16(MPUREG_ACCEL_CONFIG, BITS_FS_4G); // Accel scale g = 8192
    //    writeMPUSPIreg16(MPUREG_ACCEL_CONFIG, BITS_FS_8G); // Accel scale g = 4096
#endif
    // INT CFG => Interrupt on Data Ready, totem-pole (push-pull) output
    writeMPUSPIreg16(MPUREG_INT_PIN_CFG, BIT_INT_LEVEL | BIT_INT_RD_CLEAR); // INT: Clear on any read
    //    writeMPUSPIreg16(MPUREG_INT_PIN_CFG, BIT_INT_LEVEL | BIT_LATCH_INT_EN | BIT_INT_RD_CLEAR);
    //    writeMPUSPIreg16(MPUREG_INT_PIN_CFG, 0x1); // INT: clock out, pulse, no clear

    writeMPUSPIreg16(MPUREG_INT_ENABLE, BIT_DATA_RDY_EN); // INT: Raw data ready

#if (BOARD_TYPE == UDB4_BOARD||BOARD_TYPE == UDB5_BOARD)
    //TODO: 10MHz no longer works 31 Aug 2012: probably due to XC16 compiler
    // set prescaler for FCY/4 = 10MHz at 40MIPS
    //    initMPUSPI_master16(SEC_PRESCAL_4_1, PRI_PRESCAL_1_1);

    // set prescaler for FCY/5 = 8MHz at 40MIPS
    initMPUSPI_master16(SEC_PRESCAL_5_1, PRI_PRESCAL_1_1);

    AD1PCFGHbits.PCFG20 = 1; // Configure INT1 pin as digital
    TRISAbits.TRISA12 = 1; // make INT1 an input

#elif (BOARD_TYPE & AUAV2_BOARD)
    // set prescaler for FCY/2 = 20MHz at 40MIPS
    initMPUSPI_master16(SEC_PRESCAL_2_1, PRI_PRESCAL_1_1);

//    // set prescaler for FCY/4 = 10MHz at 40MIPS
//    initMPUSPI_master16(SEC_PRESCAL_4_1, PRI_PRESCAL_1_1);

//    // set prescaler for FCY/8 = 5MHz at 40MIPS
//    initMPUSPI_master16(SEC_PRESCAL_2_1, PRI_PRESCAL_4_1);

    //TODO: using XC16 compiler this doesn't work at 8MHz, drop to 1.25MHz
    //    initMPUSPI_master16(SEC_PRESCAL_2_1, PRI_PRESCAL_16_1);
#if ((BOARD_TYPE & AUAV2_REV) < 2)
    _TRISE8 = 1; // make INT1 an input
#else
	_TRISMPUINT = 1;
#endif

#else
#error "Invalid BOARD_TYPE for MPU6000"
#endif

#if ( MPU_SPI == 1) 
    _INT1EP = 1; // Setup INT1 pin to interrupt on falling edge
    _INT1IF = 0; // Reset INT1 interrupt flag
    _INT1IE = 1; // Enable INT1 Interrupt Service Routine 
    _INT1IP = 6;
#elif ( MPU_SPI == 2 )
    _INT3EP = 1; // Setup INT3 pin to interrupt on falling edge
    _INT3IF = 0; // Reset INT3 interrupt flag
    _INT3IE = 1; // Enable INT3 Interrupt Service Routine 
    _INT3IP = 6;
#endif

}

void process_MPU_data(void)
{
   	mpuDAV = true;
    //LED_BLUE = LED_OFF;

    udb_xaccel.value = mpu_data[xaccel_MPU_channel];
   	udb_yaccel.value = mpu_data[yaccel_MPU_channel];
    udb_zaccel.value = mpu_data[zaccel_MPU_channel];

    udb_xrate.value = mpu_data[xrate_MPU_channel];
    udb_yrate.value = mpu_data[yrate_MPU_channel];
    udb_zrate.value = mpu_data[zrate_MPU_channel];
/*
//	when we are ready to run the IMU at 200 Hz, turn this back on
	if (dcm_flags._.calib_finished) {
		dcm_run_imu_step() ;
	}
*/

	return ;
}


void MPU6000_read(void) {
    // this is working
    //    d1 = readSPI1reg16(MPUREG_INT_PIN_CFG);
    //        delay_us(10); // without delay

    // burst read guarantees that all registers represent the same sample interval
    mpuCnt++;
    readMPUSPI_burst16n(mpu_data, 7, MPUREG_ACCEL_XOUT_H , &process_MPU_data );
}

#if ( MPU_SPI == 1 )
void __attribute__((interrupt, no_auto_psv)) _INT1Interrupt(void) {
    indicate_loading_inter;
    interrupt_save_set_corcon;
    _INT1IF = 0; // Clear the INT1 interrupt flag

    //LED_BLUE = LED_ON;
    MPU6000_read();
    interrupt_restore_corcon;
	return ;
}
#elif ( MPU_SPI == 2 )
void __attribute__((interrupt, no_auto_psv)) _INT3Interrupt(void) {
    indicate_loading_inter;
    interrupt_save_set_corcon;
    _INT3IF = 0; // Clear the INT1 interrupt flag

    //LED_BLUE = LED_ON;
    MPU6000_read();
    interrupt_restore_corcon;
	return ;
}
#else
#error("invalid selection for MPU SPI port, must be 1 or 2")
#endif

void MPU6000_print(void) {
    printf("%06u axyz %06i %06i %06i gxyz %06i %06i %06i t %u\r\n",
            mpuCnt, mpu_data[0], mpu_data[1], mpu_data[2], mpu_data[4], mpu_data[5], mpu_data[6], mpu_data[3]);
}

