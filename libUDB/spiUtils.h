// file spiUtils.h

#ifndef SPIUTILS_H
#define SPIUTILS_H

//	initialize SPI in master mode, 16 bit
void initSPI1_master16(uint16_t, uint16_t);
void initSPI2_master16(uint16_t, uint16_t);

//	16 bit SPI blocking write
void writeSPI1reg16(uint16_t addr, uint16_t data);
void writeSPI2reg16(uint16_t addr, uint16_t data);

//	n-word, non-blocking SPI read, followed by call_back
void readSPI1_burst16n(uint16_t data[], int16_t n, uint16_t addr, void (* call_back)(void));
void readSPI2_burst16n(uint16_t data[], int16_t n, uint16_t addr, void (* call_back)(void));

#ifndef FCY
/* For __delay_us and __delay_ms                 */
#define FCY (FREQOSC/2)
#endif
#include <libpic30.h>        

#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */


#endif
