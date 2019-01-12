#include "stm8s.h"
#include "stdio.h"
#include "CBuff.h"

static uint8_t bUARTInited = 0;
void usart_init(uint32_t baudrate)
{
  //usart rx-->pd6
  GPIO_Init(GPIOD, GPIO_PIN_6, GPIO_MODE_IN_PU_NO_IT);
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART1, ENABLE);  //enable uart clock.
  UART1_Init(baudrate, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, UART1_PARITY_NO, UART1_SYNCMODE_CLOCK_DISABLE, UART1_MODE_RX_ENABLE|UART1_MODE_TX_DISABLE); // enable tx and rx.
  UART1_ITConfig(UART1_IT_RXNE_OR, ENABLE);
  UART1_Cmd(ENABLE);
}

void uart_char(char c)
{
  if(bUARTInited == 0)
  {
    bUARTInited = 1;
    usart_init(230400);
  }
  UART1_SendData8(c);
  /* Loop until the end of transmission */
  while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET);
}

void uart_set_baudrate(uint32_t baudrate)
{
  //firstly disable uart, if it's enabled.
  UART1_Cmd(DISABLE);
  usart_init(baudrate);
  UART1_Cmd(ENABLE);
}

extern CBuff_st comm_cbuff;
void uart_rx_isr(void)
{
  UART1_ClearFlag(UART1_FLAG_RXNE);
  CBuff_Write(&comm_cbuff, UART1->DR);
}


