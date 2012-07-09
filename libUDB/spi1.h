#ifndef __SPI1_H__
#define __SPI1_H__


#define SPI_Init SPI1_Init
#define Write8 SPI1_Write8
#define Read8  SPI1_Read8
#define Read16 SPI1_Read16
#define SPI_Get16 SPI1_Get16
#define ChipSelect SPI1_ChipSelect

void SPI1_Init(void);
void SPI1_ChipSelect(int state);
unsigned int SPI1_Write8(unsigned int add, unsigned int val);
unsigned int SPI1_Read8(unsigned int add);
unsigned int SPI1_Read16(unsigned int add);



// MPU6000 compatibility layer
#define HIGH 1
#define LOW 0
#define uint8_t unsigned char

#define SPI_transfer SPI1_Transfer

uint8_t SPI1_Transfer(uint8_t mem_rw);
unsigned int SPI1_Get16(void);
uint8_t _SPI_read(uint8_t reg);
void _SPI_write(uint8_t reg, uint8_t data);
//void _set_mpu_memory(uint8_t bank, uint8_t address, uint8_t num, uint8_t regs[]);


#endif // __SPI1_H__
