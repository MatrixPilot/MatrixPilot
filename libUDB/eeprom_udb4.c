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
//
// Based on code from Microchip AppNote1100, by Martin Bowman.
//    http://ww1.microchip.com/downloads/en/AppNotes/AN1100.zip

#include "libUDB_internal.h"


#ifdef AUAV3
#define SCL			PORTGbits.RG2		// I2C Clock PORTG pin 2
#define SDA			PORTGbits.RG3		// I2C Data PORTG pin 3
#define SCL_TRIS	TRISGbits.TRISG2	// SCL Tris bit
#define	SDA_TRIS	TRISGbits.TRISG3	// SDA Tris bit
#else
#define SCL			PORTAbits.RA2		// I2C Clock PORTG pin 2
#define SDA			PORTAbits.RA3		// I2C Data PORTG pin 3
#define SCL_TRIS	TRISAbits.TRISA2	// SCL Tris bit
#define	SDA_TRIS	TRISAbits.TRISA3	// SDA Tris bit
#endif


#define CONTROLBYTE	0b10100000			// Control byte
#define ACKBIT		0x00				// ACK bit
#define NAKBIT		0x80				// NAK bit


/** V A R I A B L E S **********************************************/
uint8_t eeprom_control = CONTROLBYTE ;	// Control byte variable


void udb_eeprom_init( void )
{
//	TRISGbits.TRISG2 = 0;		//SCL Line make Output.
	SCL_TRIS = 0;				//SCL Line make Output.
	SDA_TRIS = 1;				//SDA Line make Input.
	
	return ;
}



// Function Prototypes
void bit_in(uint8_t *data);		// Bit Input function
void bit_out(uint8_t data);		// Bit Out function
void bstart(void);						// Start condition
void bstop(void);						// Stop condition
uint8_t byte_out(uint8_t);	// Byte output
uint8_t byte_in(uint8_t);	// Byte input
void ACK_Poll(void);                    // Acknowledge polling


/********************************************************************
 * Function:        void bstart(void)
 *
 * Description:     This function generates an I2C Start condition.
 *******************************************************************/
void bstart(void)
{
    SDA_TRIS = 1;                   // Ensure SDA is high
    SCL = 1;                        // Ensure SCL is high
	Nop();
    SDA_TRIS = 0;                   // Configure SDA as an output
    SDA = 0;                        // Pull SDA low
	Nop();
    SCL = 0;                        // Pull SCL low
    
}


/********************************************************************
 * Function:        void bstop(void)
 *
 * Description:     This function generates an I2C Stop condition.
 *******************************************************************/
void bstop(void)
{
    SCL = 0;                        // Ensure SCL is low
    SDA_TRIS = 0;                   // Configure SDA as an output
    SDA = 0;                        // Ensure SDA low
	Nop();
    SCL = 1;                        // Pull SCL high
	Nop();
    SDA_TRIS = 1;                   // Allow SDA to be pulled high
}


/********************************************************************
 * Function:        void bit_out(uint8_t data)
 *
 * Description:     This function outputs a bit to the I2C bus.
 *******************************************************************/
void bit_out(uint8_t data)
{
    SCL = 0;                        // Ensure SCL is low
    if (data & 0x80)                // Check if next bit is high
    {
        SDA_TRIS = 1;               // Release SDA to be pulled high
		Nop();
		Nop();
    }
    else
    {
        SDA_TRIS = 0;               // Configure SDA as an output
        SDA = 0;                    // Pull SDA low
		Nop();
		Nop();
    }
    SCL = 1;                        // Pull SCL high to clock bit
	Nop();
	Nop();
	Nop();
    SCL = 0;                        // Pull SCL low for next bit
}


/********************************************************************
 * Function:        void bit_in(uint8_t *data)
 *
 * Description:     This function inputs a bit from the I2C bus.
 *******************************************************************/
void bit_in(uint8_t *data)
{
    SCL = 0;                        // Ensure SCL is low	
	Nop();
    SDA_TRIS = 1;                   // Configure SDA as an input
	Nop();
    SCL = 1;                        // Bring SCL high to begin transfer
    *data &= 0xFE;                  // Assume next bit is low
    if (SDA)                        // Check if SDA is high
    {
        *data |= 0x01;              // If high, set next bit
    }
	Nop();
    SCL = 0;                        // Bring SCL low again
}


/********************************************************************
 * Function:        uint8_t byte_out(uint8_t data)
 *
 * Description:     This function outputs a byte to the I2C bus.
 *                  It also receives the ACK bit and returns 0 if
 *                  successfully received, or 1 if not.
 *******************************************************************/
uint8_t byte_out(uint8_t data)
{
    uint8_t i;                // Loop counter
    uint8_t ack;              // ACK bit

    ack = 0;
    for (i = 0; i < 8; i++)         // Loop through each bit
    {
        bit_out(data);              // Output bit
        data = data << 1;           // Shift left for next bit
    }
    bit_in(&ack);                   // Input ACK bit

    return ack;
}


/********************************************************************
 * Function:        uint8_t byte_in(uint8_t ack)
 *
 * Description:     This function inputs a byte from the I2C bus.
 *                  Depending on the value of ack, it will also
 *                  transmit either an ACK or a NAK bit.
 *******************************************************************/
uint8_t byte_in(uint8_t ack)
{
    uint8_t i;                // Loop counter
    uint8_t retval;           // Return value

    retval = 0;
    for (i = 0; i < 8; i++)         // Loop through each bit
    {
        retval = retval << 1;       // Shift left for next bit
        bit_in(&retval);            // Input bit
    }
    bit_out(ack);                   // Output ACK/NAK bit

    return retval;
}


/********************************************************************
 * Function:        void ACK_Poll(void)
 *
 * Description:     This function implements Acknowledge polling.
 *******************************************************************/
void ACK_Poll(void)
{
    uint8_t result;           // Polling result

    result = 1;                     // Initialize result
    do
    {
        bstart();                   // Generate Start condition
        result = byte_out(eeprom_control); // Output control byte
    } while (result == 1);
    bstop();                        // Generate Stop condition
}



// Below are the eeprom functions exported as part of libUDB

void eeprom_ByteWrite(uint16_t address, uint8_t data)
{
    ACK_Poll();                     // Begin ACK polling
    bstart();                       // Generate Start condition
    byte_out(eeprom_control);              // Output control byte
    byte_out((uint8_t)(address>>8));// Output address MSB
    byte_out((uint8_t)address);// Output address LSB
    byte_out(data);                 // Output data byte
    bstop();                        // Generate Stop condition
}


void eeprom_PageWrite(uint16_t address, uint8_t *data, uint8_t numbytes)
{
    uint8_t i;                // Loop counter

    ACK_Poll();                     // Begin ACK polling
    bstart();                       // Generate Start condition
    byte_out(eeprom_control);              // Output control byte
    byte_out((uint8_t)(address>>8));// Output address MSB
    byte_out((uint8_t)address);// Output address LSB
    for (i = 0; i < numbytes; i++)  // Loop through data bytes
    {
        byte_out(data[i]);          // Output next data byte
    }
    bstop();                        // Generate Stop condition
}


void eeprom_ByteRead(uint16_t address, uint8_t *data)
{
    ACK_Poll();                     // Begin ACK polling
    bstart();                       // Generate Start condition
    byte_out(eeprom_control);              // Output control byte
    byte_out((uint8_t)(address>>8));// Output address MSB
    byte_out((uint8_t)address);// Output address LSB
    bstart();                       // Generate Start condition
    byte_out(eeprom_control | 0x01);       // Output control byte
    *data = byte_in(NAKBIT);        // Input data byte
    bstop();                        // Generate Stop condition
}


void eeprom_SequentialRead(uint16_t address, uint8_t *data, uint16_t numbytes)
{
    uint16_t i;                 // Loop counter

    ACK_Poll();                     // Begin ACK polling
    bstart();                       // Generate Start condition
    byte_out(eeprom_control);              // Output control byte
    byte_out((uint8_t)(address>>8));// Output address MSB
    byte_out((uint8_t)address);// Output address LSB
    bstart();                       // Generate Start condition
    byte_out(eeprom_control | 0x01);       // Output control byte
    for (i = 0; i < numbytes; i++)  // Loop through data bytes
    {
        if (i < (numbytes - 1))     // Check if more data will be read
        {
            data[i] = byte_in(ACKBIT); // If not last, input byte & send ACK
        }
        else
        {
            data[i] = byte_in(NAKBIT); // If last byte, input byte & send NAK
        }
    }
    bstop();                        // Generate Stop condition
}
