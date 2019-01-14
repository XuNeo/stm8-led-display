#ifndef _USART_H_
#define _USART_H_
#include "stm8s.h"

void usart_init(uint32_t baudrate, void *pcallback);
void uart_char(char c);

#endif
