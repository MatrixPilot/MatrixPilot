#ifndef UART2_H
#define UART2_H


void uart2_init(void);
void uart2_putc(char ch);
void uart2_puts(char *str);
int  uart2_getc(void);

void uart2_start_sending_data(void);
void uart2_set_rate(long rate);
int  uart2_check_rate(long rate);


#endif // UART2_H
