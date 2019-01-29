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
#include "led.h"
#include "stm8s.h"
#include "ezled.h"

//hardware related functions
#define LEDS5_ON()    GPIOA->ODR &= ~GPIO_PIN_3
#define LEDS5_OFF()   GPIOA->ODR |=  GPIO_PIN_3
#define LEDS4_ON()    GPIOB->ODR &= ~GPIO_PIN_5
#define LEDS4_OFF()   GPIOB->ODR |=  GPIO_PIN_5
#define LEDS3_ON()    GPIOA->ODR &= ~GPIO_PIN_1
#define LEDS3_OFF()   GPIOA->ODR |=  GPIO_PIN_1
#define LEDS2_ON()    GPIOB->ODR &= ~GPIO_PIN_4
#define LEDS2_OFF()   GPIOB->ODR |=  GPIO_PIN_4
#define LEDS1_ON()    GPIOC->ODR &= ~GPIO_PIN_3
#define LEDS1_OFF()   GPIOC->ODR |=  GPIO_PIN_3
#define LEDALL_OFF()  do{\
                        LEDS1_OFF();\
                        LEDS2_OFF();\
                        LEDS3_OFF();\
                        LEDS4_OFF();\
                        LEDS5_OFF();\
                      }while(0)

static void led_bsp_init(void);

static void led_light_up(uint8_t position_set, uint8_t seg_set){
//LEDA->PC5, LEDB->PC7, LEDC->PD4, LEDD->PD2, LEDE->PD1, LEDF->PC6, LEDG->PC4, LEDDP->PD3
#define LED_GPIOD_MSK (GPIO_PIN_4|GPIO_PIN_2|GPIO_PIN_1|GPIO_PIN_3)
#define LED_GPIOC_MSK (GPIO_PIN_5|GPIO_PIN_7|GPIO_PIN_6|GPIO_PIN_4)
  //the table. input index is LEDSEGA ~ LEDSEGDP
  const uint8_t table_seg2gpiod[] = {
    0, 0, GPIO_PIN_4, GPIO_PIN_2, 
    GPIO_PIN_1, 0, 0, GPIO_PIN_3,
  };
  const uint8_t table_seg2gpioc[] = {
    GPIO_PIN_5, GPIO_PIN_7, 0, 0,
    0, GPIO_PIN_6, GPIO_PIN_4, 0,
  };
  uint8_t i, mask;
  uint8_t gpioc_set, gpiod_set;

  LEDALL_OFF();
  GPIOC->ODR = GPIOC->ODR|LED_GPIOC_MSK;
  GPIOD->ODR = GPIOD->ODR|LED_GPIOD_MSK;
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
    case LEDPOS5:
      LEDS5_ON();
      break;
    default:
      break;
  }{
   GPIOC->ODR = (GPIOC->ODR|LED_GPIOC_MSK)&(~gpioc_set);
   GPIOD->ODR = (GPIOD->ODR|LED_GPIOD_MSK)&(~gpiod_set);
  }
}

static uint8_t _seg_buff[64];
ezledif_def ezledif={
  .address = 0x12,
  .count = 5,
  .pbuff = _seg_buff,
  .szbuff = 64,
  .init = led_bsp_init,
  .light = led_light_up,
};

static void led_bsp_init(void){
  uint8_t i;
  //leds1-> PC3, leds2->PB4, leds3->PA1, leds4->PB5, leds4->PA3
  //LEDA->PC5, LEDB->PC7, LEDC->PD4, LEDD->PD2, LEDE->PD1, LEDF->PC6, LEDG->PC4, LEDDP->PD3
  GPIO_Init(GPIOC, GPIO_PIN_3, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(GPIOB, GPIO_PIN_4, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(GPIOA, GPIO_PIN_1, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(GPIOB, GPIO_PIN_5, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(GPIOA, GPIO_PIN_3, GPIO_MODE_OUT_PP_LOW_FAST);
  LEDS1_OFF();
  LEDS2_OFF();
  LEDS3_OFF();
  LEDS4_OFF();
  LEDS5_OFF();
  GPIO_Init(GPIOC, (GPIO_Pin_TypeDef)(GPIO_PIN_5|GPIO_PIN_7|GPIO_PIN_6|GPIO_PIN_4),\
                    GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_Init(GPIOD, (GPIO_Pin_TypeDef)(GPIO_PIN_4|GPIO_PIN_2|GPIO_PIN_1|GPIO_PIN_3),\
                    GPIO_MODE_OUT_PP_LOW_FAST);
  ezledif.address = 0;
  for(i=0;i<12;i++)
    ezledif.address ^= *(uint8_t*)(0x4865+i);  //init the address with unique id.
}