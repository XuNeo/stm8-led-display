#include "stm8s.h"
#include "stdio.h"

static uint8_t bUARTInited = 0;
void usart_init(void)
{
  //usart tx-->pd5
  GPIO_Init(GPIOD, GPIO_PIN_5, GPIO_MODE_OUT_PP_HIGH_FAST);
  
  CLK_PeripheralClockConfig(CLK_PERIPHERAL_UART1, ENABLE);  //enable uart clock.
  //void UART1_Init(uint32_t BaudRate, UART1_WordLength_TypeDef WordLength, \
                UART1_StopBits_TypeDef StopBits, UART1_Parity_TypeDef Parity, \
                UART1_SyncMode_TypeDef SyncMode, UART1_Mode_TypeDef Mode);
  UART1_Init(115200, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, UART1_PARITY_NO, UART1_SYNCMODE_CLOCK_DISABLE, UART1_MODE_TXRX_ENABLE); // enable tx and rx.
  UART1_ITConfig(UART1_IT_RXNE, ENABLE);
  UART1_Cmd(ENABLE);
}

void uart_char(char c)
{
  if(bUARTInited == 0)
  {
    bUARTInited = 1;
    usart_init();
  }
  UART1_SendData8(c);
  /* Loop until the end of transmission */
  while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET);
}

void uart_rx_isr(void)
{
  UART1_ClearFlag(UART1_FLAG_RXNE);
}


