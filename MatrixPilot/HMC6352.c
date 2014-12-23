/*****************************************************************************
 * FileName:        HMC6352.c
 * Dependencies:    FreeRTOS
 * Processor:       PIC24, PIC32
 * Compiler:       	MPLAB C30 V3.00, MPLAB C32
 * Linker:          MPLAB LINK30, MPLAB LINK32
 *
 * Version  Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 1.0      Inspirati            18/01/10    Initial Version 
 *****************************************************************************/

#if 0

#include "HardwareProfile.h"
//#define PIC32_STARTER_KIT
//#include "db_utils.h"
#include "HMC6352.h"
#include "GenericTypeDefs.h"
#include "Compiler.h"
#include "debug.h"


#define HMC6352_I2C_TIMEOUT		2000

#define HMC6352_I2C_ADD			0x42

#define HMC6352_OPMODE_RAM		0x74
#define HMC6352_OUTPUT_RAM		0x4E
#define HMC6352_OPMODE_E2		0x08

#define HMC6352_STANDBY_MODE		0
#define HMC6352_QUERY_MODE			1
#define HMC6352_CONTINUOUS_MODE		2

#define HMC6352_AUTO_RESET	0b00010000;
#define HMC6352_1HZ			0b00000000;
#define HMC6352_5HZ			0b00100000;
#define HMC6352_10HZ		0b01000000;
#define HMC6352_20HZ		0b01100000;

#define HMC6352_MY_MODE		0b01110010;

#define HMC6352_E2_WRITE 'w'		// w (77) EEPROM - Address Data Write to EEPROM
#define HMC6352_E2_READ 'r'			// r (72) EEPROM - Address Data Read from EEPROM
#define HMC6352_RAM_WRITE 'G'		// G (47) RAM - Address Data Write to RAM Register
#define HMC6352_RAM_READ 'g'		// g (67) RAM - Address Data Read from RAM Register
#define HMC6352_SLEEP 'S'			// S (53) Enter Sleep Mode (Sleep)
#define HMC6352_WAKE 'W'			// W (57) Exit Sleep Mode (Wakeup)
#define HMC6352_UPDATE 'O'			// O (4F) Update Bridge Offsets (S/R Now)
#define HMC6352_CAL_ENTER 'C'		// C (43) Enter User Calibration Mode
#define HMC6352_CAL_EXIT 'E'		// E (45) Exit User Calibration Mode
#define HMC6352_E2_SAVE 'L'			// L (4C) Save Op Mode to EEPROM
#define HMC6352_READ 'A'			// A (41) MSB Data LSB Data Get Data. Compensate and Calculate New Heading

#define I2C_WRITE		0
#define I2C_READ		1

uint8_t HMC6352_initialised = 0;
float f_heading = 0.0f;

float get_heading(void)
{
	return f_heading;
}

int16_t i2c0_master_send(uint8_t addr, uint8_t* data, uint8_t cnt)
{
	int16_t timeout = 0;
	int16_t i;

	IdleI2C2();        					// Wait for I2C idle
	StartI2C2();    					// Create Start sequence
	while (I2C2CONbits.SEN);    		// Wait till start sequence is completed
//	MasterWriteI2C2(addr);				// Write slave address for write
	MasterWriteI2C2(0x42);				// Write slave address for write
	timeout = 0;
	while (I2C2STATbits.TBF);			// Wait till address is transmitted
	IdleI2C2();
	timeout = 0;
	while (I2C2STATbits.ACKSTAT) { if (timeout++ > 200) goto i2c_send_fail; };		// Wait for ACK

	for (i = 0; i < cnt; i++) {
		MasterWriteI2C2(data[i]);			// Write the data
		while (I2C2STATbits.TBF);			// Wait till data is transmitted
		IdleI2C2();
		timeout = 0;
		while (I2C2STATbits.ACKSTAT) { if (timeout++ > 200) goto i2c_send_fail; };		// Wait for ACK
	}
	timeout = 0;
i2c_send_fail:
	StopI2C2();				    		// Create Stop Sequence
	while (I2C2CONbits.PEN);    		// Wait till stop sequence is completed

	return timeout;
}

int16_t i2c0_master_receive(uint8_t addr, uint8_t* data, uint8_t cnt)
{
	int16_t timeout = 0;

	IdleI2C2();        					// Wait for I2C idle
	StartI2C2();    					// Create Start sequence
	while (I2C2CONbits.SEN);    		// Wait till start sequence is completed
	MasterWriteI2C2(addr | I2C_READ);	// Write slave address for write
	timeout = 0;
	while (I2C2STATbits.TBF);			// Wait till address is transmitted
	IdleI2C2();

	timeout = 0;
	while (I2C2STATbits.ACKSTAT) { if (timeout++ > 200) { goto i2c_send_fail; } };		// Wait for ACK

	MastergetsI2C2((uint16_t)cnt, data,(uint16_t)HMC6352_I2C_TIMEOUT);
	timeout = 0;

i2c_send_fail:
	StopI2C2();				    		// Create Stop Sequence
	while (I2C2CONbits.PEN);    		// Wait till stop sequence is completed

	return timeout;
}

void HMC6352_init(void)
{
	uint8_t data[3]={0,0,0};
	int16_t i = 0;
	//#define OpenI2C2(config, brg)	(I2C2CON = (config), I2C2BRG = (brg))
	
	OpenI2C2(I2C_EN, 0x0c6);  // 100 kHz based on PBCLK = 40 MHz
//	OpenI2C2(I2C_EN | I2C_SLW_DIS, 0x0c6);  // 100 kHz based on PBCLK = 40 MHz

	if (1) {
		data[0] = HMC6352_E2_READ; // Send a "Read EEPROM" command to the compass
	    if (i2c0_master_send(0x42, data, 1)) {
      		dprintf("HMC6352 Communication Error!\n"); // Self explanatory
	    }
		for (i = 0; i < 100000; i++);
	    if (i2c0_master_receive(0x42, data, 1) == 0) { // read out one byte of data
			dprintf("HMC6352 Software Version: %04x\r\n", data[0]);
		} else {
      		dprintf("HMC6352 Reading failed\r\n"); // Self explanatory
    	}
		for (i = 0; i < 100000; i++);
		data[0] = HMC6352_RAM_WRITE; // Send a "Write RAM" command to the compass
		data[1] = HMC6352_OPMODE_RAM;
		data[2] = HMC6352_MY_MODE;

	    if (i2c0_master_send(0x42, data, 3)) {
      		dprintf("HMC6352 Communication Error 2!\r\n"); // Self explanatory
	    }
		for (i = 0; i < 100000; i++);
		data[0] = 'A'; // Send a "Get Data" command to the compass
	    if (i2c0_master_send(0x42, data, 1)) {
      		dprintf("HMC6352 Communication Error 3!\r\n"); // Self explanatory
	    }
		HMC6352_initialised = 1;
	}	
}

int16_t HMC6352_proc(void)
{
	uint8_t data[2]={0,0};
	int16_t heading = -1;

	if (HMC6352_initialised) {
	    if(i2c0_master_receive(0x42,data,2)==0) { // read out the two bytes of data
			// Compute and print out a heading.
  			// Note the conversion to floating point and 0.1 deg accuracy.
      		//PRINTF1("Heading: %3.1fdeg\n\r",((data[0] << 8)+data[1])/10.0); 
			f_heading = ((data[0] << 8)+data[1])/10.0;
			heading = ((data[0] << 8)+data[1]);
		} else {
   			dprintf("HMC6352 Reading failed\r\n"); // Self explanatory
		}
   	}
	return heading;
}

#endif // 0
