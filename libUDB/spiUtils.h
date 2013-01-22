// file spiUtils.h

#ifndef SPIUTILS_H
#define SPIUTILS_H

void initSPI1_master16(unsigned int, unsigned int);
void initSPI2_master16(unsigned int, unsigned int);

void writeSPI1reg16(unsigned int addr, unsigned int data);
void writeSPI2reg16(unsigned int addr, unsigned int data);

void readSPI1_burst16n(unsigned int data[], int n, unsigned int addr , void (* call_back )( void) );
void readSPI2_burst16n(unsigned int data[], int n, unsigned int addr , void (* call_back )( void) );

#ifndef FCY
/* For __delay_us and __delay_ms                 */
#define FCY (FREQOSC/2)
#endif
#include <libpic30.h>        

#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */


#endif
