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
#include "../libDCM/matrix_vector_32_bit.h"
//#include "../libUDB/udbTypes.h"

boolean is_ICM_20689 = 0;
//uint16_t mpu_whoami = 0;

#if (BOARD_TYPE != UDB4_BOARD)

#include <spi.h>

//Sensor variables
uint16_t mpu_data[8], mpuCnt = 0;
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
#ifdef CONING_CORRECTION
	// set prescaler for FCY/24 = 1.33 MHz at 32 MIPS
	initMPUSPI_master16(SEC_PRESCAL_6_1, PRI_PRESCAL_4_1);
#else
	// set prescaler for FCY/6 = 05.333 MHz at 32 MIPS
	initMPUSPI_master16(SEC_PRESCAL_6_1, PRI_PRESCAL_1_1);
	// set prescaler for FCY/48 = 0.667 MHz at 32 MIPS
//	initMPUSPI_master16(SEC_PRESCAL_3_1, PRI_PRESCAL_16_1);
#endif // CONING_CORRECTION	
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
	
	// Which chip is this?
	uint16_t mpu_whoami = readMPUSPIreg16(MPUREG_WHOAMI) ;
	is_ICM_20689 = (mpu_whoami == WHOAMI_ICM_20689) ;
//	mpu_whoami = readMPUSPIreg16(MPUREG_WHOAMI) ;

	// Disable I2C bus (recommended on datasheet)
	writeMPUSPIreg16(MPUREG_USER_CTRL, BIT_I2C_IF_DIS);
	
	if (is_ICM_20689) {
//	if (mpu_whoami == WHOAMI_ICM_20689 || mpu_whoami == WHOAMI_ICM_20600) {
		// Disable I2C communications on the ICM_20689
		uint8_t v = readMPUSPIreg16(MPUREG_INT_PIN_CFG) | BIT_INT_RD_CLEAR | BIT_LATCH_INT_EN;
		v &= BIT_I2C_BYPASS_EN;
		writeMPUSPIreg16(MPUREG_INT_PIN_CFG, v);
	}

	// SAMPLE RATE
#ifdef CONING_CORRECTION
	writeMPUSPIreg16(MPUREG_SMPLRT_DIV, 0); // Sample_rate = 8000Hz
#else
	writeMPUSPIreg16(MPUREG_SMPLRT_DIV, 4); // Sample rate = 200Hz  Fsample= 1Khz/(N+1) = 200Hz
	// scaling & DLPF
#ifdef BUILD_OFFSET_TABLE
	writeMPUSPIreg16(MPUREG_CONFIG, BITS_DLPF_CFG_5HZ);
#else
	writeMPUSPIreg16(MPUREG_CONFIG, BITS_DLPF_CFG_188HZ);
#endif // BUILD_OFFSET_TABLE	
#endif // CONING_CORRECTION	
#if (GYRO_RANGE == 250 )
	writeMPUSPIreg16(MPUREG_GYRO_CONFIG, BITS_FS_250DPS);  // Gyro scale 250º/s
#elif (GYRO_RANGE == 500 )
	writeMPUSPIreg16(MPUREG_GYRO_CONFIG, BITS_FS_500DPS);  // Gyro scale 500º/s
#elif (GYRO_RANGE == 1000 )
	writeMPUSPIreg16(MPUREG_GYRO_CONFIG, BITS_FS_1000DPS); // Gyro scale 1000º/s
#elif (GYRO_RANGE == 2000 )
	writeMPUSPIreg16(MPUREG_GYRO_CONFIG, BITS_FS_2000DPS); // Gyro scale 2000º/s
#else
#error "Invalid GYRO_RANGE"
#endif // GYRO_RANGE 

	
	if (is_ICM_20689) {
//	if (mpu_whoami == WHOAMI_ICM_20689 || mpu_whoami == WHOAMI_ICM_20600) {
	#if (ACCEL_RANGE == 2)
		writeMPUSPIreg16(MPUREG_ACCEL_CONFIG, 0<<3); // Accel scele 2g, g = 8192
	#elif (ACCEL_RANGE == 4)
		writeMPUSPIreg16(MPUREG_ACCEL_CONFIG, 1<<3); // Accel scele 4g, g = 4096
	#elif (ACCEL_RANGE == 8)
		writeMPUSPIreg16(MPUREG_ACCEL_CONFIG, 2<<3); // Accel scele 8g, g = 2048
	#else
		#error "Invalid ACCEL_RANGE"
	#endif
	}
	else {
	#if (ACCEL_RANGE == 2)
		writeMPUSPIreg16(MPUREG_ACCEL_CONFIG, BITS_FS_2G); // Accel scele 2g, g = 8192
	#elif (ACCEL_RANGE == 4)
		writeMPUSPIreg16(MPUREG_ACCEL_CONFIG, BITS_FS_4G); // Accel scale 4g = 4096
	#elif (ACCEL_RANGE == 8)
		writeMPUSPIreg16(MPUREG_ACCEL_CONFIG, BITS_FS_8G); // Accel scale 8g = 2048
	#else
		#error "Invalid ACCEL_RANGE"
	#endif
	}

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

#define SAMPLE_HZ 8000
// compute binary calibration factors for each gyro channel
#define GGAINX_32 (double)CALIBRATIONX*(double)SCALEGYRO*(double)24*((double)RMAX*((double)1.0/(double)SAMPLE_HZ))
#define GGAINY_32 (double)CALIBRATIONY*(double)SCALEGYRO*(double)24*((double)RMAX*((double)1.0/(double)SAMPLE_HZ))
#define GGAINZ_32 (double)CALIBRATIONZ*(double)SCALEGYRO*(double)24*((double)RMAX*((double)1.0/(double)SAMPLE_HZ))

int32_t ggain_32[] =  { (double)256*(double)256*(double)GGAINX_32 , 
	(double)256*(double)256*GGAINY_32, 
	(double)256*(double)256*GGAINZ_32 };


union longww delta_coning_angle32[3] ;
int16_t theta_16[3] ;
union longww omega32[3] ;
union longww theta_32[3] ;
union longww _theta_32[3] ;
union longww omega_dt[3];
extern union longww omegagyro_filtered[];

// compute just the coning correction term
void compute_one_half_angle_cross_omega(void)
{
	// Take the integer cross product
	VectorCross_32(delta_coning_angle32,_theta_32,omega_dt );
	// The math requires a divide by 2 for this term.
	// Since a multiply by 4 would be otherwise required after an integer multiply,
	// the 1/2 combines with the 4 to require a net of a multiply by 2.
	delta_coning_angle32[0].WW *= 2 ;
	delta_coning_angle32[1].WW *= 2 ;
	delta_coning_angle32[2].WW *= 2 ;
}

// compute the sum of both terms in the equation for the rate of change of rotation angle vector
void compute_coning_adjustment(void)
{
	union longww rate ;
	union longww offset ;
	rate._.W1 = XRATE_SIGN_ORIENTED (((int16_t)mpu_data[xrate_MPU_channel]));
	rate._.W0 = 0 ;
	offset._.W1 = XRATE_SIGN_ORIENTED (((int16_t)udb_xrate.offset));
	offset._.W0 = 0 ;
	omega32[0].WW = (rate.WW>>1)-(offset.WW>>1);
	
	rate._.W1 = YRATE_SIGN_ORIENTED (((int16_t)mpu_data[yrate_MPU_channel]));
	rate._.W0 = 0 ;
	offset._.W1 = YRATE_SIGN_ORIENTED (((int16_t)udb_yrate.offset));
	offset._.W0 = 0 ;
	omega32[1].WW = (rate.WW>>1)-(offset.WW>>1);
	
	rate._.W1 = ZRATE_SIGN_ORIENTED (((int16_t)mpu_data[zrate_MPU_channel]));
	rate._.W0 = 0 ;
	offset._.W1 = ZRATE_SIGN_ORIENTED (((int16_t)udb_zrate.offset));
	offset._.W0 = 0 ;
	omega32[2].WW = (rate.WW>>1)-(offset.WW>>1);
	
	// if there are >>1 shifts in the offset compensation computations above,
	// then there must also be >>1 shifts in the filter compensation below
		
	omega32[0].WW += ((omegagyro_filtered[0].WW)>>1) ;
	omega32[1].WW += ((omegagyro_filtered[1].WW)>>1) ;
	omega32[2].WW += ((omegagyro_filtered[2].WW)>>1) ;
	
	omega_dt[0].WW = fract_32_mpy(omega32[0].WW,ggain_32[0]);
	omega_dt[1].WW = fract_32_mpy(omega32[1].WW,ggain_32[1]);
	omega_dt[2].WW = fract_32_mpy(omega32[2].WW,ggain_32[2]);
	
	compute_one_half_angle_cross_omega();
	
	_theta_32[0].WW += 	omega_dt[0].WW + delta_coning_angle32[0].WW ;
	_theta_32[1].WW += 	omega_dt[1].WW + delta_coning_angle32[1].WW ;
	_theta_32[2].WW += 	omega_dt[2].WW + delta_coning_angle32[2].WW ;	
}

void reset_coning_adjustment(void)
{
	_theta_32[0].WW = 0 ;
	_theta_32[1].WW = 0 ;
	_theta_32[2].WW = 0 ;	
}

int16_t sample_counter = 0 ;

int32_t xaccel32, yaccel32, zaccel32, temp32, xrate32, yrate32, zrate32 ;
uint32_t max_gyro = 0 ;

void compute_max_gyro(void)
{
	if (abs(udb_xrate.value)>max_gyro )
	{
		max_gyro = abs(udb_xrate.value) ;
	}
	if (abs(udb_yrate.value)>max_gyro )
	{
		max_gyro = abs(udb_yrate.value) ;
	}	
	if (abs(udb_zrate.value)>max_gyro )
	{
		max_gyro = abs(udb_zrate.value) ;
	}	
}

void __attribute__((__interrupt__,__no_auto_psv__)) _T2Interrupt(void)
{
	indicate_loading_inter;
	interrupt_save_set_corcon;
	set_ipl_on_output_pin;
	_T2IF = 0;              // clear the interrupt
	compute_max_gyro();
	if (callback) callback();
	interrupt_restore_corcon;
	unset_ipl_on_output_pin;
}

#ifndef CONING_CORRECTION

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

	_T2IF = 1; // trigger callback at a lower priority
//	if (callback) callback();   // was directly calling heartbeat()
}

#else

// executed for each of sample at the 8000 Hz sample rate
static void process_MPU_data(void)
{
	mpuDAV = true;

//	integrate all data for use in upstream calculations other than those that need coning correction	
	xaccel32 += ((int32_t)((int16_t)mpu_data[xaccel_MPU_channel])) ;
	yaccel32 += ((int32_t)((int16_t)mpu_data[yaccel_MPU_channel])) ;
	zaccel32 += ((int32_t)((int16_t)mpu_data[zaccel_MPU_channel])) ;
	
	temp32 += ((int32_t)((int16_t)mpu_data[temp_MPU_channel])) ;
	
	xrate32 += ((int32_t)((int16_t)mpu_data[xrate_MPU_channel])) ;
	yrate32 += ((int32_t)((int16_t)mpu_data[yrate_MPU_channel])) ;
	zrate32 += ((int32_t)((int16_t)mpu_data[zrate_MPU_channel])) ;

#ifdef CONING_CORRECTION	
	compute_coning_adjustment();
#endif
	//  trigger synchronous processing of sensor data
	sample_counter = sample_counter+1 ;
	// time to pass the consolidation of 40 samples up to the 200 Hz processes
	if (sample_counter == 40)
	{
		udb_xaccel.value = __builtin_divsd(xaccel32+20,40);
		udb_yaccel.value = __builtin_divsd(yaccel32+20,40);
		udb_zaccel.value = __builtin_divsd(zaccel32+20,40);

		mpu_temp.value = __builtin_divsd(temp32+20,40);

		udb_xrate.value = __builtin_divsd(xrate32+20,40);
		udb_yrate.value = __builtin_divsd(yrate32+20,40);
		udb_zrate.value = __builtin_divsd(zrate32+20,40);
		
		xaccel32 = 0 ;
		yaccel32 = 0 ;
		zaccel32 = 0 ;
		temp32 = 0 ;
		xrate32 = 0 ;
		yrate32 = 0 ;
		zrate32 = 0 ;
#ifdef 	CONING_CORRECTION
		// theta values used to update the 32 bit direction cosine matrix
		theta_32[0].WW = _theta_32[0].WW ;
		theta_32[1].WW = _theta_32[1].WW ;
		theta_32[2].WW = _theta_32[2].WW ;
		
		// round off the 32 bit theta values for the option of logging just the upper 16 bits
		_theta_32[0].WW += 0x00008000 ;
		_theta_32[1].WW += 0x00008000 ;
		_theta_32[2].WW += 0x00008000 ;
		
		theta_16[0] = _theta_32[0]._.W1 ;
		theta_16[1] = _theta_32[1]._.W1 ;
		theta_16[2] = _theta_32[2]._.W1 ;
		
		// get ready for the next batch of 40 samples
		reset_coning_adjustment();
#endif // CONING_CORRECTION		
		sample_counter = 0 ;
		// perform the 200 Hz IMU calculations
	_T2IF = 1; // trigger callback at a lower priority
//	if (callback) callback();   // was directly calling heartbeat()	if (callback) callback();   // was directl
	}
}
#endif // CONING_CORRECTION
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
