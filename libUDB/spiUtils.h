// file spiUtils.h

#ifndef SPIUTILS_H
#define SPIUTILS_H

#if 0
void initSPI1_master(unsigned int, unsigned int);
void initSPI2_slave(unsigned int, unsigned int);

void MPU6000_init(void);
void writeSPI1reg(unsigned int addr, unsigned int mpu_data);
unsigned int readSPI1reg(unsigned int addr);
unsigned int readSPI1_burst();
#endif

void initSPI1_master16(unsigned int, unsigned int);
void initSPI2_master16(unsigned int, unsigned int);

void initSPI2_slave16(unsigned int, unsigned int);
void MPU6000_init16(void);
unsigned char readSPI1reg16(unsigned int addr);

//	16 bit SPI blocking write
void writeSPI1reg16(unsigned int addr, unsigned int data);
void writeSPI2reg16(unsigned int addr, unsigned int data);


//	n-word, non-blocking SPI read, followed by call_back
void readSPI1_burst16n(unsigned int data[], int n, unsigned int addr);
void readSPI2_burst16n(unsigned int data[], int n, unsigned int addr , void (* call_back )( void) );

#define SPI1_SS   _LATB2

#ifndef FCY
/* For __delay_us and __delay_ms                 */
#define FCY (FREQOSC/2)
#endif
#include <libpic30.h>        

#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */

// Data received at SPI1,2
extern unsigned int spi1Data, spi2Data;

// flags set by ISRs
extern bool spi1DAV, spi2DAV;

#endif
