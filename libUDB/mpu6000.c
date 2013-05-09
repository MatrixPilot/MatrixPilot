// This file is part of MatrixPilot.
//
//    http://code.google.com/p/gentlenav/
//
// Copyright 2009-2012 MatrixPilot Team
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


#include "libUDB_internal.h"
#include "oscillator.h"
#include "interrupt.h"
#include "spiUtils.h"
#include "mpu6000.h"
#include "../libDCM/libDCM_internal.h"

#include "HardwareProfile.h"
#include <libpic30.h>
#include <stdbool.h>

#include <stdio.h>
#include <spi.h>


//Sensor variables
uint16_t mpu_data[8], mpuCnt = 0;
bool mpuDAV = false;

struct ADchannel udb_xaccel, udb_yaccel, udb_zaccel; // x, y, and z accelerometer channels
struct ADchannel udb_xrate, udb_yrate, udb_zrate; // x, y, and z gyro channels
struct ADchannel mpu_temp;
int16_t vref_adj;

// MPU6000 Initialization and configuration

void MPU6000_init16(void) {

    MPUSPI_SS = 1;    // deassert MPU SS
    MPUSPI_TRIS = 0; // make MPU SS  an output

#if (MIPS == 64)
	// set prescaler for FCY/?? = xxx KHz at 64MIPS
	initMPUSPI_master16(SEC_PRESCAL_6_1, PRI_PRESCAL_64_1);
#elif (MIPS == 32)
	// Use the following after we raise MatrixPilot from 16 to 40 MIPS on UDB4 and UDB5
    // set prescaler for FCY/64 = 625KHz at 40MIPS
//    initMPUSPI_master16(SEC_PRESCAL_4_1, PRI_PRESCAL_16_1);
	initMPUSPI_master16(SEC_PRESCAL_6_1, PRI_PRESCAL_4_1);
#elif (MIPS == 16)
	// set prescaler for FCY/24 = 666 KHz at 16MIPS
	initMPUSPI_master16(SEC_PRESCAL_6_1, PRI_PRESCAL_4_1);
#else
#error Invalid MIPS Configuration
#endif // MIPS

    // need at least 60 msec delay here
    __delay_ms(60);
    writeMPUSPIreg16(MPUREG_PWR_MGMT_1, BIT_H_RESET);

    // 10msec delay seems to be needed for AUAV3 (MW's prototype)
    __delay_ms(10);

    // Wake up device and select GyroZ clock (better performance)
    writeMPUSPIreg16(MPUREG_PWR_MGMT_1, MPU_CLK_SEL_PLLGYROZ);

    // Disable I2C bus (recommended on datasheet)
    writeMPUSPIreg16(MPUREG_USER_CTRL, BIT_I2C_IF_DIS);

    // SAMPLE RATE
    writeMPUSPIreg16(MPUREG_SMPLRT_DIV, 4); // Sample rate = 200Hz    Fsample= 1Khz/(N+1) = 200Hz

    // scaling & DLPF
    writeMPUSPIreg16(MPUREG_CONFIG, BITS_DLPF_CFG_42HZ);

    //	writeMPUSPIreg16(MPUREG_GYRO_CONFIG, BITS_FS_2000DPS);  // Gyro scale 2000º/s
    writeMPUSPIreg16(MPUREG_GYRO_CONFIG, BITS_FS_500DPS); // Gyro scale 500º/s

#if ( ACCEL_RANGE == 2 )
    writeMPUSPIreg16(MPUREG_ACCEL_CONFIG, BITS_FS_2G); // Accel scele 2g, g = 8192
#elif ( ACCEL_RANGE == 4 )
    writeMPUSPIreg16(MPUREG_ACCEL_CONFIG, BITS_FS_4G); // Accel scale g = 4096
#elif ( ACCEL_RANGE == 8 )
    writeMPUSPIreg16(MPUREG_ACCEL_CONFIG, BITS_FS_8G); // Accel scale g = 2048
#else
#error "Invalid ACCEL_RANGE"
#endif

#if 0
	// Legacy from Mark Whitehorn's testing, we might need it some day.
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
    writeMPUSPIreg16(MPUREG_INT_ENABLE, BIT_DATA_RDY_EN); // INT: Raw data ready

#if (BOARD_TYPE == UDB4_BOARD || BOARD_TYPE == UDB5_BOARD || BOARD_TYPE == AUAV3_BOARD)
	// UDB4_BOARD option is for testing purposes

	// When MP is revised to run at 40 MIPS, use this instead:
    // set prescaler for FCY/5 = 8MHz at 40MIPS
    // initMPUSPI_master16(SEC_PRESCAL_5_1, PRI_PRESCAL_1_1);

#if (MIPS == 64)
	// When MP is revised to run at 40 MIPS, use this instead:
    // set prescaler for FCY/5 = 8MHz at 40MIPS
    initMPUSPI_master16(SEC_PRESCAL_5_1, PRI_PRESCAL_1_1);
#elif (MIPS == 32)
	// When MP is revised to run at 40 MIPS, use this instead:
    // set prescaler for FCY/5 = 8MHz at 40MIPS
    // initMPUSPI_master16(SEC_PRESCAL_5_1, PRI_PRESCAL_1_1);
	initMPUSPI_master16(SEC_PRESCAL_2_1, PRI_PRESCAL_1_1);
#elif (MIPS == 16)
	// older versions of MatrixPilot run at 16 MIPS on UDB4 and UDB5
	// set prescaler for FCY/2 = 8MHz at 16 MIPS
	initMPUSPI_master16(SEC_PRESCAL_2_1, PRI_PRESCAL_1_1);
#else
#error Invalid MIPS Configuration
#endif // MIPS

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

    mpu_temp.value = mpu_data[temp_MPU_channel];

    udb_xrate.value = mpu_data[xrate_MPU_channel];
    udb_yrate.value = mpu_data[yrate_MPU_channel];
    udb_zrate.value = mpu_data[zrate_MPU_channel];

//{
//	static int i = 0;
//	if (i++ > 10) {
//		i = 0;
//		printf("%u %u %u\r\n", udb_xaccel.value, udb_yaccel.value, udb_zaccel.value);
//	}
//}

/*
//	This version of the MPU interface writes and reads gyro and accelerometer values asynchronously.
//  This was the fastest way to revise the software.
//	MPU data is being read at 200 Hz, IMU and control loop runs at 40 Hz.
//  4 out of 5 samples are being ignored. IMU gets the most recent set of samples.
//  Eventually, we will want to run write-read synchronously, and run the IMU at 200 Hz, using every sample.
//	When we are ready to run the IMU at 200 Hz, turn the following back on
	if (dcm_flags._.calib_finished) {
		dcm_run_imu_step() ;
	}
*/

	return ;
}


void MPU6000_read(void) {
    
    // burst read guarantees that all registers represent the same sample interval
    mpuCnt++;
	// Non-blocking read of 7 words of data from MPU, starting with X acceleration, and then call process_MPU_data
    readMPUSPI_burst16n(mpu_data, 7, MPUREG_ACCEL_XOUT_H , &process_MPU_data );
}

#if ( MPU_SPI == 1 )
void __attribute__((interrupt, no_auto_psv)) _INT1Interrupt(void) {
    _INT1IF = 0; // Clear the INT1 interrupt flag
    indicate_loading_inter;
    interrupt_save_set_corcon(INT1_INT, 0);
    MPU6000_read();
    interrupt_restore_corcon(INT1_INT, 0);
	return ;
}

#elif ( MPU_SPI == 2 )
void __attribute__((interrupt, no_auto_psv)) _INT3Interrupt(void) {
    _INT3IF = 0; // Clear the INT3 interrupt flag
    indicate_loading_inter;
    interrupt_save_set_corcon(INT3_INT, 0);
    MPU6000_read();
    interrupt_restore_corcon(INT3_INT, 0);
	return ;
}
#else
#error("invalid selection for MPU SPI port, must be 1 or 2")
#endif

// Used for debugging:
void MPU6000_print(void) {
    printf("%06u axyz %06i %06i %06i gxyz %06i %06i %06i t %u\r\n",
            mpuCnt, mpu_data[0], mpu_data[1], mpu_data[2], mpu_data[4], mpu_data[5], mpu_data[6], mpu_data[3]);
}

