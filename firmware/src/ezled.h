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
#ifndef _EZLED_H_
#define _EZLED_H_
#include "stm8s.h"

#define MAX_LED_NUM 8    //maximum LED number.

#define LEDSEGA     0x01
#define LEDSEGB     0x02
#define LEDSEGC     0x04
#define LEDSEGD     0x08
#define LEDSEGE     0x10
#define LEDSEGF     0x20
#define LEDSEGG     0x40
#define LEDSEGDP    0x80

typedef enum{
  LEDPOS1 = 0x01,
  LEDPOS2 = 0x02,
  LEDPOS3 = 0x04,
  LEDPOS4 = 0x08,
  LEDPOS5 = 0x10,
  LEDPOS6 = 0x20,
  LEDPOS7 = 0x40,
  LEDPOS8 = 0x80,
  LEDPOSNON = 0,
}ledpos_def;

typedef enum{
  LED_SPEED0 = 0,  //lowest speed
  LED_SPEED1,
  LED_SPEED2,
  LED_SPEED3,
  LED_SPEED4,
  LED_SPEED5,
  LED_SPEED6,
  LED_SPEED7,
  LED_SPEED8,
  LED_SPEED9,      //highest speed
}led_speed_def;

/* ezLedLib interface definination*/

typedef struct _ezled_if{
  uint8_t         count;                /* How many numbers can display on led screen */
  uint8_t *       pbuff;                /* The raw data to display on LED directly */
  uint8_t         szbuff;               /* Size of provided buffer. */
  void            (*init)   (void);     /* Init LED related hardwares */
  void            (*light)  (uint8_t position, uint8_t seg_set);
}ezledif_def;

typedef struct{
  uint8_t c;
  uint8_t font;
}led_font_def;

typedef struct _ezled{
  ezledif_def         *ezledif;       /* The hardware interface */
  uint8_t             charlen;        /* Character length not including '.' */
  uint8_t             blink_pos_set;  /* Private variables */
  led_speed_def       blink_speed;    /* Blink speed. */
  led_speed_def       scroll_speed;
  //ledcont_def         led_contrast;
  uint8_t             contrast[2][MAX_LED_NUM];    /**< contrast value for each led. [0] is default value. 
                                      [1] is used for blink. The contrast value will blink between [0] and [1] */
  led_font_def        *pfontbuf;      /**< user programmable font */
  uint8_t             buffsz;         /**< font buffer size. */
  uint8_t             fontcount;      /**< user font count. */
  struct{ //private variables
    uint8_t curr_pos;
    uint8_t contrast_sel;   /**< if corresponding bit is set, this led will use contrast[1] otherwise [0]. Used for blink */
    uint8_t count_pre_div;  //the counter for pre-divider.
    uint8_t count_blink_div;    
    uint8_t count_scroll_div;
    uint8_t count_contrast;
    uint8_t scroll_en;      //enable scroll function(when content to display exceed led count.).
    uint8_t scroll_pos;     //curr start position to disp buffer(ezledif.pbuff).
  }private;
}ezled_def;

int8_t ezled_init(ezled_def* pezled, ezledif_def*phardware);
void ezled_poll(ezled_def* pezled);
void ezled_set_fontbuf(ezled_def* pezled, uint8_t *pbuff, uint32_t buffsz);
int8_t ezled_font_append(ezled_def* pezled, const led_font_def *pchar);
void ezled_print(ezled_def* pezled, char *pstr);
void ezled_set_blink(ezled_def *pezled, uint8_t pos_set);
void ezled_set_blink_speed(ezled_def *pezled, led_speed_def speed);
void ezled_set_scroll_speed(ezled_def *pezled, led_speed_def speed);
void ezled_set_contrastA(ezled_def *pezled, uint8_t pos_set, uint8_t contrast);
void ezled_set_contrastB(ezled_def *pezled, uint8_t pos_set, uint8_t contrast);
void ezled_timer_isr(ezled_def *pezled);

#endif
