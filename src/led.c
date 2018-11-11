#include "led.h"
#include "stm8s.h"

#define LEDS1_ON()    GPIOC->ODR &= ~GPIO_PIN_3
#define LEDS1_OFF()   GPIOC->ODR |=  GPIO_PIN_3
#define LEDS2_ON()    GPIOB->ODR &= ~GPIO_PIN_4
#define LEDS2_OFF()   GPIOB->ODR |=  GPIO_PIN_4
#define LEDS3_ON()    GPIOA->ODR &= ~GPIO_PIN_3
#define LEDS3_OFF()   GPIOA->ODR |=  GPIO_PIN_3
#define LEDS4_ON()    GPIOD->ODR &= ~GPIO_PIN_5
#define LEDS4_OFF()   GPIOD->ODR |=  GPIO_PIN_5
#define LEDALL_OFF()  do{\
                        LEDS1_OFF();\
                        LEDS2_OFF();\
                        LEDS3_OFF();\
                        LEDS4_OFF();\
                      }while(0)
#define LED_A   GPIO_PIN_4
#define LED_B   GPIO_PIN_2
#define LED_C   GPIO_PIN_7
#define LED_D   GPIO_PIN_5
#define LED_E   GPIO_PIN_4
#define LED_F   GPIO_PIN_3
#define LED_G   GPIO_PIN_1
#define LED_DP  GPIO_PIN_6

#define LED_GPIOD_MSK (GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4)
#define LED_GPIOC_MSK (GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7)

typedef enum
{
  LED_SEG1,
  LED_SEG2,
  LED_SEG3,
  LED_SEG4,
}led_seg_def;

const uint8_t num_table_gpioc[]={
  LED_GPIOC_MSK&(LED_A|LED_B|LED_C|LED_D|LED_E|LED_F|LED_G|LED_DP),  /* 0 */
};
const uint8_t num_table_gpiod[]={
  LED_GPIOD_MSK&(LED_A|LED_B|LED_C|LED_D|LED_E|LED_F|LED_G|LED_DP),  /* 0 */
};

void led_light_up(led_seg_def which_seg, uint8_t num)
{
  LEDALL_OFF();
  switch(which_seg)
  {
    case LED_SEG1:
      LEDS1_ON();
    break;
    case LED_SEG2:
      LEDS2_ON();
    break;
    case LED_SEG3:
      LEDS3_ON();
    break;
    case LED_SEG4:
      LEDS4_ON();
    break;
  }
      LEDS1_ON();
      LEDS2_ON();
      LEDS3_ON();
      LEDS4_ON();
  GPIOC->ODR = (GPIOC->ODR|LED_GPIOC_MSK)&(~num_table_gpioc[num]);
  GPIOD->ODR = (GPIOD->ODR|LED_GPIOD_MSK)&(~num_table_gpiod[num]);
}

/**
 * @brief init all peripherals used to drive LED segment.
 * @return none.
*/
void led_init(void)
{
  //leds1-> PC3, leds2->PB4, leds3->PA3, leds4->PD5
  //LEDA->PD4, LEDB->PD2, LEDC->PC7, LEDD->PC5, LEDE->PC4, LEDF->PD3, LEDG->PD1, LEDDP->PC6
  GPIO_Init(GPIOC, GPIO_PIN_3, GPIO_MODE_OUT_PP_HIGH_FAST);
  GPIO_Init(GPIOB, GPIO_PIN_4, GPIO_MODE_OUT_PP_HIGH_FAST);
  GPIO_Init(GPIOA, GPIO_PIN_3, GPIO_MODE_OUT_PP_HIGH_FAST);
  GPIO_Init(GPIOD, GPIO_PIN_5, GPIO_MODE_OUT_PP_HIGH_FAST);
  LEDS1_OFF();
  LEDS2_OFF();
  LEDS3_OFF();
  LEDS4_OFF();
  GPIO_Init(GPIOC, (GPIO_Pin_TypeDef)(GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7),\
                    GPIO_MODE_OUT_PP_HIGH_FAST);
  GPIO_Init(GPIOD, (GPIO_Pin_TypeDef)(GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4),\
                    GPIO_MODE_OUT_PP_HIGH_FAST);
  led_light_up(LED_SEG1, 0);
}
