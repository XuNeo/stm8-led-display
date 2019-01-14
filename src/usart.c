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
  volatile uint8_t c = (uint8_t)UART1->DR;
  UART1_ClearFlag(UART1_FLAG_RXNE);
  if(UART1_GetFlagStatus(UART1_FLAG_OR) == SET)
    ;
  //CBuff_Write(&comm_cbuff, UART1->DR);
  if(usart_rx_callback)
    usart_rx_callback(c);
}


