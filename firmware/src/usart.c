/**
 * @author Neo Xu (neo.xu1990@gmail.com)
 * @license The MIT License (MIT)
 * 
 * Copyright (c) 2019 Neo Xu
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
*/
#include "stm8s.h"
#include "stdio.h"

static void (*usart_rx_callback)(uint8_t ch) = 0;  //the callback function when uart received character.

void usart_init(uint32_t baudrate, void *pcallback)
{
  //usart rx-->pd6
  GPIO_Init(GPIOD, GPIO_PIN_6, GPIO_MODE_IN_PU_NO_IT);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART1, ENABLE);  //enable uart clock.
  UART1_Init(baudrate, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, UART1_PARITY_NO, UART1_SYNCMODE_CLOCK_DISABLE, UART1_MODE_RX_ENABLE|UART1_MODE_TX_DISABLE); // enable tx and rx.
  UART1_ITConfig(UART1_IT_RXNE_OR, ENABLE);
  UART1_Cmd(ENABLE);
  usart_rx_callback = (void (*)(uint8_t))pcallback;
}

void uart_char(char c)
{
  UART1_SendData8(c);
  /* Loop until the end of transmission */
  while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET);
}

void uart_set_baudrate(uint32_t baudrate)
{
  //firstly disable uart, if it's enabled.
  UART1_Cmd(DISABLE);
  usart_init(baudrate, (void *)usart_rx_callback);  //do not change callback function.
  UART1_Cmd(ENABLE);
}

void uart_rx_isr(void)
{
  volatile uint8_t c;
  //UART1_ClearFlag(UART1_FLAG_RXNE);
  c = UART1->SR;
  c = (uint8_t)UART1->DR; //this way can clear flag OR and RXNE.
  //if(usart_rx_callback)
    usart_rx_callback(c);
}


