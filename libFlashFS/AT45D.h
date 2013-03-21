#ifndef DATAFLASH_H
#define DATAFLASH_H


void init_dataflash(void);

void Page_To_Buffer(uint16_t PageAdr, uint8_t BufferNo);
void Buffer_To_Page(uint8_t BufferNo, uint16_t PageAdr);

uint8_t Buffer_Read_Byte(uint8_t BufferNo, uint16_t IntPageAdr);
void Buffer_Write_Byte(uint8_t BufferNo, uint16_t IntPageAdr, uint8_t Data);

void BufferReadStr(uint8_t BufferNo, uint16_t IntPageAdr, uint16_t No_of_uint8_ts, uint8_t *BufferPtr);
void BufferWriteStr(uint8_t BufferNo, uint16_t IntPageAdr, uint16_t No_of_uint8_ts, uint8_t *BufferPtr);

void BufferToPage(uint8_t BufferNo, uint16_t PageAdr);
void PageToBuffer(uint16_t PageAdr, uint8_t BufferNo);

#endif // DATAFLASH_H
