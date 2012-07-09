#include "spi1.h"
#include "delay.h"
#include <spi.h>


#define SPI1_SS   PORTBbits.RB2

void SPI1_Init(void)
{
	TRISFbits.TRISF6 = 0;   // make SCK1 an output
	TRISFbits.TRISF8 = 0;   // make SDO1 an output
	TRISBbits.TRISB2 = 0;   // make SS1  an output
	TRISFbits.TRISF7 = 1;   // make SDI1 an input
   
	OpenSPI1(ENABLE_SCK_PIN & ENABLE_SDO_PIN & SPI_MODE16_OFF & SPI_SMP_OFF & SPI_CKE_OFF &
			SLAVE_ENABLE_OFF & MASTER_ENABLE_ON & PRI_PRESCAL_16_1,
			FRAME_ENABLE_OFF,
			SPI_ENABLE & SPI_RX_OVFLOW_CLR);
}

void SPI1_ChipSelect(int state)
{
	SPI1_SS = state;
}

uint8_t SPI1_Transfer(uint8_t mem_rw)
{
	uint8_t i;

	i = SPI1BUF;
	SPI1BUF = mem_rw;
	while(SPI1STATbits.SPITBF) ;
	while(!SPI1STATbits.SPIRBF) ;
	i = SPI1BUF;

	return i;
}

unsigned int SPI1_Get16(void)
{
	return (SPI1_Transfer(0) << 8) | (SPI1_Transfer(0) & 0xFF);
}      

unsigned int SPI1_Read16(unsigned int register_name)
{
	unsigned int i;
	unsigned int j;
       
	register_name <<= 2;
	register_name &= 0b11111100; //Read command
       
	//SPI1_SS = 0;
	//delay_us(10);
	SPI1_SS = 0;
	SPI1_Transfer(register_name);
       
	i = SPI1_Transfer(0);
	i <<= 8;
	j = SPI1_Transfer(0);
	SPI1_SS = 1;

	i |= (j & 0xFF);
       
	return i;
}      

unsigned int SPI1_Read8(unsigned int add)
{
        unsigned int i;
       
        add <<= 1;
        add |= 0x00;  // read
        add <<= 1;
        add |= 0x00;
       
        while(SPI1STATbits.SPITBF) ;
       
        i = SPI1BUF;

        SPI1_SS = 0;
        delay_us(10);
       
        SPI1BUF = add;
        while(SPI1STATbits.SPITBF) ;    
        while(!SPI1STATbits.SPIRBF) ;
        delay_us(1);
        i = SPI1BUF;

        SPI1BUF = 0x00;
        while(SPI1STATbits.SPITBF) ;
        while(!SPI1STATbits.SPIRBF) ;
        delay_us(1);
        i = SPI1BUF;
        SPI1_SS = 1;
       
        return i;
}      

unsigned int SPI1_Write8(unsigned int add, unsigned int val)
{
	int i;

	add <<= 1;
	add |= 0x01;  // read	
	add <<= 1;
	add |= 0x00;

	SPI1_SS = 0;
	delay_us(10);
	SPI1_Transfer(add);
	i = SPI1_Transfer(val);
	SPI1_SS = 1;
	return i;
}


// MPU6000 compatibility layer

uint8_t _SPI_read(uint8_t reg)
{
	uint8_t data;

	ChipSelect(LOW);
	SPI1_Transfer(reg | 0x80); // Set most significant bit
	data = SPI1_Transfer(0);
	ChipSelect(HIGH);

	return data;
}

void _SPI_write(uint8_t reg, uint8_t data)
{
	ChipSelect(LOW);
	SPI1_Transfer(reg);
	SPI1_Transfer(data);
	ChipSelect(HIGH);  
}
