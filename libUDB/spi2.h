#ifndef __SPI2_H__
#define __SPI2_H__


#define SPI_Init SPI2_Init
#define Write8 SPI2_Write8
#define Read8  SPI2_Read8
#define Read16 SPI2_Read16
#define SPI_Get16 SPI2_Get16
#define ChipSelect SPI2_ChipSelect

void SPI2_Init(void);
void SPI2_ChipSelect(int state);
unsigned int SPI2_Write8(unsigned int add, unsigned int val);
unsigned int SPI2_Read8(unsigned int add);
unsigned int SPI2_Read16(unsigned int add);


/*
// MPU6000 compatibility layer
#define HIGH 2
#define LOW 0
#define uint8_t unsigned char

#define SPI_transfer SPI2_Transfer

uint8_t SPI2_Transfer(uint8_t mem_rw);
unsigned int SPI2_Get16(void);
uint8_t _SPI_read(uint8_t reg);
void _SPI_write(uint8_t reg, uint8_t data);
//void _set_mpu_memory(uint8_t bank, uint8_t address, uint8_t num, uint8_t regs[]);
 */

#endif // __SPI2_H__
