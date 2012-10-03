#include "spi2.h"
#include "delay.h"
#include <spi.h>

//#define USE_SPI2

#ifdef USE_SPI2

#define SPI2_SS   PORTGbits.RG9

void SPI2_Init(void)
{
        TRISGbits.TRISG6 = 0;   // make SCK2 an output
        TRISGbits.TRISG8 = 0;   // make SDO2 an output
        TRISGbits.TRISG9 = 0;   // make SS2  an output
        TRISGbits.TRISG7 = 1;   // make SDI2 an input
   
	OpenSPI2(ENABLE_SCK_PIN & ENABLE_SDO_PIN & SPI_MODE16_OFF & SPI_SMP_OFF & SPI_CKE_OFF &
			SLAVE_ENABLE_OFF & MASTER_ENABLE_ON & PRI_PRESCAL_16_1,
			FRAME_ENABLE_OFF,
			SPI_ENABLE & SPI_RX_OVFLOW_CLR);
}

void SPI2_ChipSelect(int state)
{
	SPI2_SS = state;
}

uint8_t SPI2_Transfer(uint8_t mem_rw)
{
	uint8_t i;

	i = SPI2BUF;
	SPI2BUF = mem_rw;
	while(SPI2STATbits.SPITBF) ;
	while(!SPI2STATbits.SPIRBF) ;
	i = SPI2BUF;

	return i;
}

unsigned int SPI2_Get16(void)
{
	return (SPI2_Transfer(0) << 8) | (SPI2_Transfer(0) & 0xFF);
}      

unsigned int SPI2_Read16(unsigned int register_name)
{
	unsigned int i;
	unsigned int j;
       
	register_name <<= 2;
	register_name &= 0b11111100; //Read command
       
	//SPI2_SS = 0;
	//delay_us(10);
	SPI2_SS = 0;
	SPI2_Transfer(register_name);
       
	i = SPI2_Transfer(0);
	i <<= 8;
	j = SPI2_Transfer(0);
	SPI2_SS = 1;

	i |= (j & 0xFF);
       
	return i;
}      

unsigned int SPI2_Read8(unsigned int add)
{
        unsigned int i;
       
        add <<= 1;
        add |= 0x00;  // read
        add <<= 1;
        add |= 0x00;
       
        while(SPI2STATbits.SPITBF) ;
       
        i = SPI2BUF;

        SPI2_SS = 0;
        delay_us(10);
       
        SPI2BUF = add;
        while(SPI2STATbits.SPITBF) ;    
        while(!SPI2STATbits.SPIRBF) ;
        delay_us(1);
        i = SPI2BUF;

        SPI2BUF = 0x00;
        while(SPI2STATbits.SPITBF) ;
        while(!SPI2STATbits.SPIRBF) ;
        delay_us(1);
        i = SPI2BUF;
        SPI2_SS = 1;
       
        return i;
}      

unsigned int SPI2_Write8(unsigned int add, unsigned int val)
{
	int i;

	add <<= 1;
	add |= 0x01;  // read	
	add <<= 1;
	add |= 0x00;

	SPI2_SS = 0;
	delay_us(10);
	SPI2_Transfer(add);
	i = SPI2_Transfer(val);
	SPI2_SS = 1;
	return i;
}


// MPU6000 compatibility layer
/*
uint8_t _SPI_read(uint8_t reg)
{
	uint8_t data;

	ChipSelect(LOW);
	SPI2_Transfer(reg | 0x80); // Set most significant bit
	data = SPI2_Transfer(0);
	ChipSelect(HIGH);

	return data;
}

void _SPI_write(uint8_t reg, uint8_t data)
{
	ChipSelect(LOW);
	SPI2_Transfer(reg);
	SPI2_Transfer(data);
	ChipSelect(HIGH);  
}
 */

#endif // USE_SPI2
