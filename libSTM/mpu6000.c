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


#include "../libUDB/libUDB.h"
#include "../libUDB/oscillator.h"
#include "../libUDB/interrupt.h"
#include "../libUDB/heartbeat.h"
#include "../libUDB/ADchannel.h"
#include "mpu_spi.h"
#include "mpu6000.h"
//#include "../libDCM/libDCM_internal.h"

#if (BOARD_TYPE != UDB4_BOARD)
//#include <spi.h>

//Sensor variables
uint16_t mpu_data[8], mpuCnt = 0;
boolean mpuDAV = false;

//struct ADchannel udb_xaccel, udb_yaccel, udb_zaccel; // x, y, and z accelerometer channels
//struct ADchannel udb_xrate,  udb_yrate,  udb_zrate;  // x, y, and z gyro channels
struct ADchannel mpu_temp;
int16_t vref_adj;

extern SPI_HandleTypeDef hspi2;

// MPU6000 Initialization and configuration

static callback_fptr_t callback = NULL;

void MPU6000_init16(callback_fptr_t fptr)
{
//	callback = fptr;

// MPU-6000 maximum SPI clock is specified as 1 MHz for all registers
//    however the datasheet states that the sensor and interrupt registers
//    may be read using an SPI clock of 20 Mhz

// As these register accesses are one time only during initial setup lets be
//    conservative and only run the SPI bus at half the maximum specified speed

	HAL_StatusTypeDef err;
//	uint8_t data[8]={0,0,0,0,0,0,0,0};
//	uint16_t dataOut, dataIn=0;
	//	func_SPI_Write_Byte((MPUREG_PWR_MGMT_1|0x80),0x00, data);
	//
	//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
	//	data[0] = MPUREG_PWR_MGMT_1;
	//	err=HAL_SPI_Transmit(&hspi2, data, 1, 10);
	//	data[0] = BIT_H_RESET;
	//	err=HAL_SPI_Transmit(&hspi2, data, 1, 10);
	//    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);

	//	writeMPUSPIreg16(MPUREG_PWR_MGMT_1, BIT_H_RESET);
	//	func_SPI_Write_Byte(MPUREG_PWR_MGMT_1, BIT_H_RESET,data);
	//	func_SPI_Write_Byte((MPUREG_PWR_MGMT_1|0x80),0x00, data);

	// need at least 60 msec delay here
	HAL_Delay(60);
	err = writeMPUSPIreg16(MPUREG_PWR_MGMT_1, BIT_H_RESET);
	if(err != HAL_OK){
		//TODO: Do something with posible error here
		while(1);
	}
	// 10msec delay seems to be needed for AUAV3 (MW's prototype)
	HAL_Delay(10);
	// Wake up device and select GyroZ clock (better performance)
	err = writeMPUSPIreg16(MPUREG_PWR_MGMT_1, MPU_CLK_SEL_PLLGYROZ);
	if(err != HAL_OK){
		//TODO: Do something with posible error here
		while(1);
	}
//	func_SPI_Write_Byte((MPUREG_PWR_MGMT_1|0x80),0x00, data);
	// Disable I2C bus (recommended on datasheet)
	err = writeMPUSPIreg16(MPUREG_USER_CTRL, BIT_I2C_IF_DIS);
	if(err != HAL_OK){
		//TODO: Do something with posible error here
		while(1);
	}
	// SAMPLE RATE
	err = writeMPUSPIreg16(MPUREG_SMPLRT_DIV, 4); // Sample rate = 200Hz  Fsample= 1Khz/(N+1) = 200Hz
	if(err != HAL_OK){
		//TODO: Do something with posible error here
		while(1);
	}
	// scaling & DLPF
	err = writeMPUSPIreg16(MPUREG_CONFIG, BITS_DLPF_CFG_42HZ);
	if(err != HAL_OK){
		//TODO: Do something with posible error here
		while(1);
	}

//	writeMPUSPIreg16(MPUREG_GYRO_CONFIG, BITS_FS_2000DPS);  // Gyro scale 2000º/s
	err = writeMPUSPIreg16(MPUREG_GYRO_CONFIG, BITS_FS_500DPS); // Gyro scale 500º/s
	if(err != HAL_OK){
		//TODO: Do something with posible error here
		while(1);
	}

#if (ACCEL_RANGE == 2)
	err = writeMPUSPIreg16(MPUREG_ACCEL_CONFIG, BITS_FS_2G); // Accel scele 2g, g = 8192
	if(err != HAL_OK){
		//TODO: Do something with posible error here
		while(1);
	}
#elif (ACCEL_RANGE == 4)
	err = writeMPUSPIreg16(MPUREG_ACCEL_CONFIG, BITS_FS_4G); // Accel scale g = 4096
	if(err != HAL_OK){
		//TODO: Do something with posible error here
		while(1);
	}
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

	// INT CFG => Interrupt on Data Ready, totem-pole (push-pull) output INT: Clear on any read
	err = writeMPUSPIreg16(MPUREG_INT_PIN_CFG, BIT_INT_LEVEL | BIT_INT_RD_CLEAR);
	if(err != HAL_OK){
		//TODO: Do something with posible error here
		while(1);
	}
	// INT: Raw data ready
	err = writeMPUSPIreg16(MPUREG_INT_ENABLE, BIT_DATA_RDY_EN);
	if(err != HAL_OK){
		//TODO: Do something with posible error here
		while(1);
	}

// Bump the SPI clock up towards 10.5 MHz for ongoing sensor and interrupt register reads
// 20 MHz is the maximum specified for the MPU-6000
//
//TODO: Check if hspi is initialized ok and just need to change BaudRatePrescaler

//	hspi2.Instance = SPI2;
//	hspi2.Init.Mode = SPI_MODE_MASTER;
//	hspi2.Init.Direction = SPI_DIRECTION_2LINES;
//	hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
//	hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
//	hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
//	hspi2.Init.NSS = SPI_NSS_HARD_OUTPUT;
//	hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
//	hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
//	hspi2.Init.TIMode = SPI_TIMODE_DISABLED;
//	hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
//	HAL_SPI_Init(&hspi2);

//NOTE: Should I enable INT here? It's already enabled, but maybe I should move here.
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);
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
	if (callback) callback();   // was directly calling heartbeat()
#else
#warning mpu6000: no callback mechanism defined
#endif // (BOARD_TYPE != UDB4_BOARD && HEARTBEAT_HZ == 200)
}

static inline void MPU6000_read(void)
{
	// burst read guarantees that all registers represent the same sample interval
	mpuCnt++;
	// Non-blocking read of 7 words of data from MPU, starting with X acceleration, and then call process_MPU_data
	readMPUSPI_burst16n(mpu_data, 7, MPUREG_ACCEL_XOUT_H, &process_MPU_data);
}
/*
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
 */
// Used for debugging:
void MPU6000_print(void)
{
	printf("%06u axyz %06i %06i %06i gxyz %06i %06i %06i t %u\r\n",
	    mpuCnt,      mpu_data[0], mpu_data[1], mpu_data[2],
	    mpu_data[4], mpu_data[5], mpu_data[6], mpu_data[3]);
}

#endif // (BOARD_TYPE != UDB4_BOARD)
