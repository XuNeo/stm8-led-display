#include "led.h"
#include "stm8s.h"
#include "ezled.h"

//hardware related functions
#define LEDS4_ON()    GPIOC->ODR &= ~GPIO_PIN_3
#define LEDS4_OFF()   GPIOC->ODR |=  GPIO_PIN_3
#define LEDS3_ON()    GPIOB->ODR &= ~GPIO_PIN_4
#define LEDS3_OFF()   GPIOB->ODR |=  GPIO_PIN_4
#define LEDS2_ON()    GPIOA->ODR &= ~GPIO_PIN_3
#define LEDS2_OFF()   GPIOA->ODR |=  GPIO_PIN_3
#define LEDS1_ON()    GPIOD->ODR &= ~GPIO_PIN_5
#define LEDS1_OFF()   GPIOD->ODR |=  GPIO_PIN_5
#define LEDALL_OFF()  do{\
                        LEDS1_OFF();\
                        LEDS2_OFF();\
                        LEDS3_OFF();\
                        LEDS4_OFF();\
                      }while(0)

static void led_bsp_init(void){
  //leds1-> PC3, leds2->PB4, leds3->PA3, leds4->PD5
  //LEDA->PD4, LEDB->PD2, LEDC->PC7, LEDD->PC5, LEDE->PC4, LEDF->PD3, LEDG->PD1, LEDDP->PC6
  GPIO_Init(GPIOC, GPIO_PIN_3, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(GPIOB, GPIO_PIN_4, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(GPIOA, GPIO_PIN_3, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(GPIOD, GPIO_PIN_5, GPIO_MODE_OUT_PP_LOW_FAST);
  LEDS1_OFF();
  LEDS2_OFF();
  LEDS3_OFF();
  LEDS4_OFF();
  GPIO_Init(GPIOC, (GPIO_Pin_TypeDef)(GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7),\
                    GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(GPIOD, (GPIO_Pin_TypeDef)(GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4),\
                    GPIO_MODE_OUT_PP_LOW_FAST);  
  /* Time base configuration */
  TIM2_TimeBaseInit(TIM2_PRESCALER_1, 999);
  TIM2_ARRPreloadConfig(ENABLE);
  /* TIM2 enable counter */
  TIM2_Cmd(ENABLE);
  TIM2_ITConfig(TIM2_IT_UPDATE, ENABLE);
}

static void led_light_up(uint8_t position_set, uint8_t seg_set){
//LEDA->PD4, LEDB->PD2, LEDC->PC7, LEDD->PC5, LEDE->PC4, LEDF->PD3, LEDG->PD1, LEDDP->PC6
#define LED_GPIOD_MSK (GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4)
#define LED_GPIOC_MSK (GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7)
  //the table. input index is LEDSEGA ~ LEDSEGDP
  const uint8_t table_seg2gpiod[] = {
    GPIO_PIN_4, GPIO_PIN_2, 0, 0,
    0, GPIO_PIN_3, GPIO_PIN_1, 0,
  };
  const uint8_t table_seg2gpioc[] = {
    0, 0, GPIO_PIN_7, GPIO_PIN_5,
    GPIO_PIN_4, 0, 0, GPIO_PIN_6,
  };
  uint8_t i, mask;
  uint8_t gpioc_set, gpiod_set;

  LEDALL_OFF();
  {int i=1;while(i--);} /* Wait until the current falls to zero before turn on next seg. */
  /* Get the gpio setting from seg_set. */
  gpioc_set = 0;
  gpiod_set = 0;
  mask = 0x01;  /* Corresponding to SEGA */
  for(i=0;i<8;i++){
    if(seg_set&mask){
      gpioc_set |= table_seg2gpioc[i];
      gpiod_set |= table_seg2gpiod[i];
    }
    mask <<= 1;
  }{
   GPIOC->ODR = (GPIOC->ODR|LED_GPIOC_MSK)&(~gpioc_set);
   GPIOD->ODR = (GPIOD->ODR|LED_GPIOD_MSK)&(~gpiod_set);
  }
  switch(position_set){
    case LEDPOS1:
      LEDS1_ON();
      break;
    case LEDPOS2:
      LEDS2_ON();
      break;
    case LEDPOS3:
      LEDS3_ON();
      break;
    case LEDPOS4:
      LEDS4_ON();
      break;
    default:
      break;
  }
}

static uint8_t _seg_buff[32];
ezledif_def ezledif={
  .phook = 0,
  .count = 4,
  .pbuff = _seg_buff,
  .szbuff = 32,
  .init = led_bsp_init,
  .light = led_light_up,
};

void timer2_isr(void){
  TIM2_ClearITPendingBit(TIM2_IT_UPDATE);
  ezled_timer_isr(ezledif.phook); //do this quickly and exit isr quickly.
}