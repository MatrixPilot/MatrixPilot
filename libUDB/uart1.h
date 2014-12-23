#ifndef UART1_H
#define UART1_H


//void uart1_init(void);
int16_t  uart1_getc(void);
void uart1_putc(char ch);
void uart1_puts(char *str);
uint16_t write(int16_t handle, void *buffer, uint16_t len);

//void uart1_start_sending_data(void);
//void uart1_set_rate(int32_t rate);
//int  uart1_check_rate(int32_t rate);


#endif // UART1_H
