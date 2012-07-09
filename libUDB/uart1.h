#ifndef UART1_H
#define UART1_H


void uart1_init(void);
void uart1_putc(char ch);
void uart1_puts(char *str);
int  uart1_getc(void);

void uart1_start_sending_data(void);
void uart1_set_rate(long rate);
int  uart1_check_rate(long rate);


#endif // UART1_H
