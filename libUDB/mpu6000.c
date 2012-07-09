// Internal MPU6000 axis definition
// X axis pointing to right, Y axis pointing forward and Z axis pointing up

#include "libUDB_internal.h"
#include "delay.h"
#include "spi1.h"
#include "mpu6000.h"
#include <stdio.h>


//Sensor variables
int accelX;
int accelY;
int accelZ;
unsigned int mpu_temp;
int gyroX;
int gyroY;
int gyroZ;


// MPU6000 Initialization and configuration
void MPU6000_init(void)
{
	SPI_Init();

	delay_us(10);
	ChipSelect(HIGH);
	delay_us(100);

	// Chip reset
	_SPI_write(MPUREG_PWR_MGMT_1, BIT_H_RESET);
	delay_us(100);
	delay_us(100);

	_SPI_write(MPUREG_AUX_VDDIO, 0); // Auxiliary I2C Supply Selection, set to zero for MPU-6000

	// Wake up device and select GyroZ clock (better performance)
	_SPI_write(MPUREG_PWR_MGMT_1, MPU_CLK_SEL_PLLGYROZ);
	// Disable I2C bus (recommended on datasheet)
	_SPI_write(MPUREG_USER_CTRL, BIT_I2C_IF_DIS);
	// SAMPLE RATE
	_SPI_write(MPUREG_SMPLRT_DIV, 0x04);     // Sample rate = 200Hz    Fsample= 1Khz/(4+1) = 200Hz     
	// FS & DLPF   FS=2000º/s, DLPF = 42Hz (low pass filter)
	_SPI_write(MPUREG_CONFIG, BITS_DLPF_CFG_42HZ);  // BITS_DLPF_CFG_20HZ BITS_DLPF_CFG_42HZ BITS_DLPF_CFG_98HZ
//	_SPI_write(MPUREG_GYRO_CONFIG, BITS_FS_2000DPS);  // Gyro scale 2000º/s
	_SPI_write(MPUREG_GYRO_CONFIG, BITS_FS_500DPS);  // Gyro scale 500º/s
//	_SPI_write(MPUREG_ACCEL_CONFIG, BITS_FS_2G);           // Accel scele 2g (g=8192)  
	_SPI_write(MPUREG_ACCEL_CONFIG, BITS_FS_8G);           // Accel scele 8g

	// INT CFG => Interrupt on Data Ready
//	_SPI_write(MPUREG_INT_ENABLE, BIT_RAW_RDY_EN);         // INT: Raw data ready
	_SPI_write(MPUREG_INT_ENABLE, BIT_DATA_RDY_EN);         // INT: Raw data ready
//	_SPI_write(MPUREG_INT_PIN_CFG, BIT_INT_ANYRD_2CLEAR);  // INT: Clear on any read
//	_SPI_write(MPUREG_INT_PIN_CFG, BIT_INT_LEVEL | BIT_LATCH_INT_EN | BIT_INT_RD_CLEAR);
	_SPI_write(MPUREG_INT_PIN_CFG, BIT_INT_LEVEL | BIT_INT_RD_CLEAR);

	// Oscillator set
	// _SPI_write(MPUREG_PWR_MGMT_1,MPU_CLK_SEL_PLLGYROZ);

//#define INT1        PORTAbits.RA12
//#define INT1_TRIS   TRISAbits.TRISA12

#if (BOARD_TYPE == UDB4_BOARD)
	TRISAbits.TRISA12 = 1;	// make INT1 an input
#endif

#if (BOARD_TYPE == UDB5_BOARD)
	TRISEbits.TRISE8 = 1;	// make INT1 an input
#endif

	INTCON2bits.INT1EP = 1;	// Setup INT1 pin to interrupt on falling edge
	IFS1bits.INT1IF = 0;    // Reset INT1 interrupt flag
//	IEC1bits.INT1IE = 1;    // Enable INT1 Interrupt Service Routine
}

void MPU6000_print(void)
{
	printf( "axyz %06i %06i %06i gxyz %06i %06i %06i t %u\r\n", accelX, accelY, accelZ, gyroX, gyroY, gyroZ, mpu_temp);
}

// Read raw data (gyros, temperature and accel) sensors on MPU6000
void MPU6000_read(void)
{
	// We start a SPI multibyte read of sensors
	uint8_t addr = MPUREG_ACCEL_XOUT_H | 0x80;      // Set most significant bit
	ChipSelect(LOW);
	SPI_transfer(addr);
	accelX = SPI_Get16();
	accelY = SPI_Get16();
	accelZ = SPI_Get16();
	mpu_temp = SPI_Get16();
	gyroX = SPI_Get16();
	gyroY = SPI_Get16();
	gyroZ = SPI_Get16();
	ChipSelect(HIGH);
}

void __attribute__((interrupt, no_auto_psv)) _INT1Interrupt(void)
{
	indicate_loading_inter ;
	interrupt_save_set_corcon ;

//	LED_BLUE = LED_ON;//	MPU6000_read();
//	LED_BLUE = LED_OFF;
	IFS1bits.INT1IF = 0;    // Clear the INT1 interrupt flag

	interrupt_restore_corcon ;
}
