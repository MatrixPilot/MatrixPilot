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


#include "libUDB.h"
#include "oscillator.h"
#include "interrupt.h"
#include "heartbeat.h"
#include "ADchannel.h"
#include "mpu_spi.h"
#include "mpu6000.h"

#if (BOARD_TYPE != UDB4_BOARD)

#include <spi.h>

//Sensor variables
uint16_t mpu_data[8], mpuCnt = 0, mpuOverSample = 1;
boolean mpuDAV = false;

//struct ADchannel udb_xaccel, udb_yaccel, udb_zaccel; // x, y, and z accelerometer channels
//struct ADchannel udb_xrate,  udb_yrate,  udb_zrate;  // x, y, and z gyro channels
struct ADchannel mpu_temp;
int16_t vref_adj;

// MPU6000 Initialization and configuration

static callback_fptr_t callback = NULL;

void MPU6000_init16(callback_fptr_t fptr)
{
	callback = fptr;

// MPU-6000 maximum SPI clock is specified as 1 MHz for all registers
//    however the datasheet states that the sensor and interrupt registers
//    may be read using an SPI clock of 20 Mhz
//    NOTE!!: the SPI limit on the dsPIC is 9 Mhz

// Primary prescaler options   1:1/4/16/64
// Secondary prescaler options 1:1 to 1:8

// As these register accesses are one time only during initial setup lets be
//    conservative and only run the SPI bus at half the maximum specified speed

#if (MIPS == 70)
	// set prescaler for FCY/112 = 625 kHz at 70MIPS
	initMPUSPI_master16(SEC_PRESCAL_7_1, PRI_PRESCAL_16_1);
#elif (MIPS == 64)
	// set prescaler for FCY/96 = 667 kHz at 64MIPS
	initMPUSPI_master16(SEC_PRESCAL_6_1, PRI_PRESCAL_16_1);
#elif (MIPS == 40)
	// set prescaler for FCY/64 = 625 KHz at 40MIPS
	initMPUSPI_master16(SEC_PRESCAL_4_1, PRI_PRESCAL_16_1);
#elif (MIPS == 32)
	// set prescaler for FCY/48 = 667 kHz at 32 MIPS
	initMPUSPI_master16(SEC_PRESCAL_3_1, PRI_PRESCAL_16_1);
#elif (MIPS == 16)
	// set prescaler for FCY/24 = 667 kHz at 16MIPS
	initMPUSPI_master16(SEC_PRESCAL_6_1, PRI_PRESCAL_4_1);
#else
#error Invalid MIPS Configuration
#endif // MIPS

	// need at least 60 msec delay here
	delay_ms(60);
	writeMPUSPIreg16(MPUREG_PWR_MGMT_1, BIT_H_RESET);

	// 10msec delay seems to be needed for AUAV3 (MW's prototype)
	delay_ms(10);

	// Wake up device and select GyroZ clock (better performance)
	writeMPUSPIreg16(MPUREG_PWR_MGMT_1, MPU_CLK_SEL_PLLGYROZ);

	// Disable I2C bus (recommended on datasheet)
	writeMPUSPIreg16(MPUREG_USER_CTRL, BIT_I2C_IF_DIS);

	// SAMPLE RATE
// Modif gfm Gyro : sample rate growing up to 1 KHz with DLPF <> 0 and 7 to keep Fsample=1kHz
//	writeMPUSPIreg16(MPUREG_SMPLRT_DIV, 4); // Sample rate = 200Hz  Fsample= 1kHz/(N+1) = 200Hz
	writeMPUSPIreg16(MPUREG_SMPLRT_DIV, 0); // Sample rate = 1000Hz  Fsample= 1kHz/(N+1) = 1000Hz
         mpuOverSample = 1000 / HEARTBEAT_HZ;
// Take care : due to this sample rate, dcm_run_imu_step()must be called in libDCM
//             and not here under.
//        A filter can be used to limit propeller vibrations (max frequency 10 000 RPM=166 Hz
// fin modif gfm gyro
	// scaling & DLPF
	writeMPUSPIreg16(MPUREG_CONFIG, BITS_DLPF_CFG_42HZ);

//	writeMPUSPIreg16(MPUREG_GYRO_CONFIG, BITS_FS_2000DPS);  // Gyro scale 2000º/s
	writeMPUSPIreg16(MPUREG_GYRO_CONFIG, BITS_FS_500DPS); // Gyro scale 500º/s

#if (ACCEL_RANGE == 2)
	writeMPUSPIreg16(MPUREG_ACCEL_CONFIG, BITS_FS_2G); // Accel scele 2g, g = 8192
#elif (ACCEL_RANGE == 4)
	writeMPUSPIreg16(MPUREG_ACCEL_CONFIG, BITS_FS_4G); // Accel scale g = 4096
#elif (ACCEL_RANGE == 8)
	writeMPUSPIreg16(MPUREG_ACCEL_CONFIG, BITS_FS_8G); // Accel scale g = 2048
#else
#error "Invalid ACCEL_RANGE"
#endif

#if 0
	// Legacy from Mark Whitehorn's testing, we might need it some day.
	// SAMPLE RATE
	writeMPUSPIreg16(MPUREG_SMPLRT_DIV, 7); // Sample rate = 1KHz  Fsample= 8Khz/(N+1)

	// no DLPF, gyro sample rate 8KHz
	writeMPUSPIreg16(MPUREG_CONFIG, BITS_DLPF_CFG_256HZ_NOLPF2);

	writeMPUSPIreg16(MPUREG_GYRO_CONFIG, BITS_FS_500DPS); // Gyro scale 500º/s

//	writeMPUSPIreg16(MPUREG_ACCEL_CONFIG, BITS_FS_2G); // Accel scale 2g, g = 16384
	writeMPUSPIreg16(MPUREG_ACCEL_CONFIG, BITS_FS_4G); // Accel scale g = 8192
//	writeMPUSPIreg16(MPUREG_ACCEL_CONFIG, BITS_FS_8G); // Accel scale g = 4096
#endif

	// INT CFG => Interrupt on Data Ready, totem-pole (push-pull) output
	writeMPUSPIreg16(MPUREG_INT_PIN_CFG, BIT_INT_LEVEL | BIT_INT_RD_CLEAR); // INT: Clear on any read
	writeMPUSPIreg16(MPUREG_INT_ENABLE, BIT_DATA_RDY_EN); // INT: Raw data ready

// Bump the SPI clock up towards 20 MHz for ongoing sensor and interrupt register reads
// 20 MHz is the maximum specified for the MPU-6000
// however 9 MHz is the maximum specified for the dsPIC33EP
// Primary prescaler options   1:1/4/16/64
// Secondary prescaler options 1:1 to 1:8
#if (MIPS == 70)
	// set prescaler for FCY/32 = 2.2 MHz at 70MIPS
	initMPUSPI_master16(SEC_PRESCAL_2_1, PRI_PRESCAL_16_1);
#elif (MIPS == 64)
	// set prescaler for FCY/8 = 8 MHz at 64 MIPS
	initMPUSPI_master16(SEC_PRESCAL_2_1, PRI_PRESCAL_4_1);
#elif (MIPS == 40)
	// UDB5 only
	// set prescaler for FCY/5 = 8 MHz at 40MIPS
	initMPUSPI_master16(SEC_PRESCAL_5_1, PRI_PRESCAL_1_1);
#elif (MIPS == 32)
	// set prescaler for FCY/4 = 8 MHz at 32 MIPS
	initMPUSPI_master16(SEC_PRESCAL_1_1, PRI_PRESCAL_4_1);
#elif (MIPS == 16)
	// set prescaler for FCY/2 = 8 MHz at 16 MIPS
	initMPUSPI_master16(SEC_PRESCAL_2_1, PRI_PRESCAL_1_1);
#else
#error Invalid MIPS Configuration
#endif // MIPS

	_TRISMPUINT = 1; // this is probably already taken care of in mcu.c for most boards

#if (MPU_SPI == 1)
	_INT1EP = 1; // Setup INT1 pin to interrupt on falling edge
	_INT1IP = INT_PRI_INT1;
	_INT1IF = 0; // Reset INT1 interrupt flag
	_INT1IE = 1; // Enable INT1 Interrupt Service Routine 
#elif (MPU_SPI == 2)
	_INT3EP = 1; // Setup INT3 pin to interrupt on falling edge
	_INT3IP = INT_PRI_INT3;
	_INT3IF = 0; // Reset INT3 interrupt flag
	_INT3IE = 1; // Enable INT3 Interrupt Service Routine 
#endif
}

static void process_MPU_data(void)
{
	mpuDAV = true;

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

#if (BOARD_TYPE != UDB4_BOARD && HEARTBEAT_HZ == 200)
	//  trigger synchronous processing of sensor data
	if (mpuCnt % mpuOverSample == 0)
            if (callback) callback();   // was directly calling heartbeat()
#else
#warning mpu6000: no callback mechanism defined
#endif // (BOARD_TYPE != UDB4_BOARD && HEARTBEAT_HZ == 200)
}

static void MPU6000_read(void)
{
	// burst read guarantees that all registers represent the same sample interval
	mpuCnt++;
	// Non-blocking read of 7 words of data from MPU, starting with X acceleration, and then call process_MPU_data
	readMPUSPI_burst16n(mpu_data, 7, MPUREG_ACCEL_XOUT_H, &process_MPU_data);
}

#if (MPU_SPI == 1)
void __attribute__((interrupt, no_auto_psv)) _INT1Interrupt(void)
{
	_INT1IF = 0; // Clear the INT1 interrupt flag
	indicate_loading_inter;
	interrupt_save_set_corcon;
	MPU6000_read();
	interrupt_restore_corcon;
}
#elif (MPU_SPI == 2)
void __attribute__((interrupt, no_auto_psv)) _INT3Interrupt(void)
{
	_INT3IF = 0; // Clear the INT3 interrupt flag
	indicate_loading_inter;
	interrupt_save_set_corcon;
	MPU6000_read();
	interrupt_restore_corcon;
}
#else
#error("invalid selection for MPU SPI port, must be 1 or 2")
#endif

// Used for debugging:
void MPU6000_print(void)
{
	printf("%06u axyz %06i %06i %06i gxyz %06i %06i %06i t %u\r\n",
	    mpuCnt,      mpu_data[0], mpu_data[1], mpu_data[2], 
	    mpu_data[4], mpu_data[5], mpu_data[6], mpu_data[3]);
}

#endif // (BOARD_TYPE != UDB4_BOARD)
