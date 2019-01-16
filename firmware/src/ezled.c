#include "ezled.h"
#include "stm8s.h"

/**
 * The font table
*/
static const led_font_def num_font[]={
  {'0', LEDSEGC|LEDSEGD|LEDSEGE|LEDSEGA|LEDSEGB|LEDSEGF,          },/* 0 */
  {'1', LEDSEGB|LEDSEGC,                                          },/* 1 */
  {'2', LEDSEGE|LEDSEGD|LEDSEGA|LEDSEGB|LEDSEGG,                  },/* 2 */
  {'3', LEDSEGC|LEDSEGD|LEDSEGA|LEDSEGB|LEDSEGG,                  },/* 3 */
  {'4', LEDSEGC|LEDSEGB|LEDSEGF|LEDSEGG,                          },/* 4 */
  {'5', LEDSEGC|LEDSEGD|LEDSEGA|LEDSEGF|LEDSEGG,                  },/* 5 */
  {'6', LEDSEGC|LEDSEGD|LEDSEGE|LEDSEGA|LEDSEGF|LEDSEGG,          },/* 6 */
  {'7', LEDSEGC|LEDSEGA|LEDSEGB,                                  },/* 7 */
  {'8', LEDSEGC|LEDSEGD|LEDSEGE|LEDSEGA|LEDSEGB|LEDSEGF|LEDSEGG,  },/* 8 */
  {'9', LEDSEGC|LEDSEGA|LEDSEGB|LEDSEGF|LEDSEGG,                  },/* 9 */
  {'a', LEDSEGC|LEDSEGE|LEDSEGA|LEDSEGB|LEDSEGF|LEDSEGG,          },/* a */
  {'A', LEDSEGC|LEDSEGE|LEDSEGA|LEDSEGB|LEDSEGF|LEDSEGG,          },/* A */
  {'b', LEDSEGC|LEDSEGD|LEDSEGE|LEDSEGF|LEDSEGG,                  },/* b */
  {'B', LEDSEGC|LEDSEGD|LEDSEGE|LEDSEGF|LEDSEGG,                  },/* b */
  {'c', LEDSEGD|LEDSEGE|LEDSEGG,                                  },/* c */
  {'C', LEDSEGD|LEDSEGE|LEDSEGF|LEDSEGA,                          },/* C */
  {'d', LEDSEGC|LEDSEGD|LEDSEGE|LEDSEGB|LEDSEGG,                  },/* d */
  {'D', LEDSEGC|LEDSEGD|LEDSEGE|LEDSEGB|LEDSEGG,                  },/* d */
  {'e', LEDSEGD|LEDSEGE|LEDSEGA|LEDSEGB|LEDSEGF|LEDSEGG           },/* e */
  {'E', LEDSEGD|LEDSEGE|LEDSEGA|LEDSEGF|LEDSEGG,                  },/* E */
  {'f', LEDSEGE|LEDSEGA|LEDSEGF|LEDSEGG,                          },/* f */
  {'F', LEDSEGE|LEDSEGA|LEDSEGF|LEDSEGG,                          },/* f */
  {'g', LEDSEGC|LEDSEGD|LEDSEGA|LEDSEGB|LEDSEGF|LEDSEGG,          },/* g */
  {'h', LEDSEGF|LEDSEGE|LEDSEGG|LEDSEGC,                          },/* h */
  {'H', LEDSEGC|LEDSEGE|LEDSEGB|LEDSEGF|LEDSEGG,                  },/* H */
  {'l', LEDSEGE|LEDSEGF,                                          },/* l */
  {'L', LEDSEGD|LEDSEGE|LEDSEGF,                                  },/* L */
  {'o', LEDSEGC|LEDSEGD|LEDSEGE|LEDSEGG,                          },/* o */
  {'O', LEDSEGC|LEDSEGD|LEDSEGE|LEDSEGA|LEDSEGB|LEDSEGF,          },/* O */
  {'p', LEDSEGE|LEDSEGA|LEDSEGB|LEDSEGG|LEDSEGF,                  },/* P */
  {'-', LEDSEGG,                                                  },/* - */
  {' ', 0,                                                        },/* LED Dark */
};

static uint8_t led_font_count = sizeof(num_font)/sizeof(led_font_def);
/**
 * Contrast table, the value is for internal use. 
 * It set the off duty of selected seg. the unit is timer isr period.
*/
static const uint8_t contrast_table[10]={
  200,100,50,25,15,10,8,6,4,0,
};

/**
 * @brief ezled main loop. call this function periodically.
 * @return none.
*/
void ez_led_poll(ezled_def* pezled){
  if(pezled->flag_interrupt == 0) return;
  pezled->flag_interrupt = 0; //clear interrupt flag.
  if(pezled->private.curr_pos < pezled->ezledif->count){
    uint8_t pos2index = 1<<pezled->private.curr_pos;
    if(pos2index & pezled->private.disp_en){ //if this led is allowed to display.
      //fetch the content to display.
      uint8_t led;
      if(pezled->private.scroll_en){
        uint8_t buff_index = pezled->private.curr_pos + pezled->private.scroll_pos;
        if(buff_index >= pezled->charlen)
          led = 0;
        else
          led = pezled->ezledif->pbuff[buff_index];
      }
      else{
        if(pezled->charlen == 0)
          led = 0;  //no display data.
        else{
          if(pezled->private.curr_pos > pezled->charlen-1)
            led = 0;  //dark if there is no data.
          else
            led = pezled->ezledif->pbuff[pezled->private.curr_pos]; //get data.
        }
      }
      pezled->ezledif->light(pos2index, led);
    }
    else
      pezled->ezledif->light(LEDPOSNON, 0);
  }
  else{//shut down all
    pezled->ezledif->light(LEDPOSNON, 0);
  }
  pezled->private.curr_pos ++;
  if(pezled->private.curr_pos > contrast_table[pezled->led_contrast] + pezled->ezledif->count)
    pezled->private.curr_pos = 0;
  /* Do blink */
  pezled->private.count_pre_div++;
  if(pezled->private.count_pre_div == 50){
    pezled->private.count_pre_div = 0;
    if(pezled->blink_pos_set){ //not empty
      pezled->private.count_blink_div++;
      if(pezled->private.count_blink_div > (LED_SPEED9 +1 - pezled->blink_speed)*20){
        pezled->private.count_blink_div = 0;
        pezled->private.disp_en ^= pezled->blink_pos_set;
      }
    }
    if(pezled->private.scroll_en){
      pezled->private.count_scroll_div++;
      if(pezled->private.count_scroll_div > (LED_SPEED9 +1 - pezled->scroll_speed)*20){
        pezled->private.count_scroll_div = 0;
        //adjust scroll position.
        pezled->private.scroll_pos ++;
        if(pezled->private.scroll_pos >= pezled->charlen)
          pezled->private.scroll_pos = 0;
      }
    }
  }
}

/**
 * @brief timer isr function. call this function in timer isr.
 * @return none.
*/
void ezled_timer_isr(ezled_def *pezled){
  pezled->flag_interrupt = 1;
}

/**
 * @brief set which seg to blink.
 * @param pos_set: bit-set to select which seg to blink.\
 * @return none.
*/
void ezled_set_blink(ezled_def *pezled, uint8_t pos_set){
  if(pezled == 0) return;
  pezled->blink_pos_set = pos_set;
  pezled->private.disp_en = 0xff;
}

/**
 * @brief set blink speed. 10 levels available.
 * @param speed: select from @ref led_speed_def
 * @return none.
*/
void ezled_set_blink_speed(ezled_def *pezled, led_speed_def speed){
  if(pezled == 0) return;
  pezled->blink_speed = speed;
}

/**
 * @brief set scroll speed. 10 levels available.
 * @param speed: select from @ref led_speed_def
 * @return none.
*/
void ezled_set_scroll_speed(ezled_def *pezled, led_speed_def speed){
  if(pezled == 0) return;
  pezled->scroll_speed = speed;
}

/**
 * @brief set led contrast. 10 levels available.
 * @param contrast: select from @ref ledcont_def
 * @return none.
*/
void ezled_set_contrast(ezled_def *pezled, ledcont_def contrast){
  if(pezled == 0) return;
  pezled->led_contrast = contrast;
}

/**
 * @brief print string to led. String will be truncated by number of led.
 * @param pstr: ponter to string.
*/
void ezled_print(ezled_def* pezled, char *pstr){
  uint8_t index = 0;
  pezled->charlen = 0;
  while(*pstr){
    uint8_t i;
    if(*pstr == '.'){
      if(index)
        pezled->ezledif->pbuff[index-1] |= LEDSEGDP;
    }
    else{
      pezled->ezledif->pbuff[index] = 0;
      for(i=0;i<led_font_count;i++)
        if(*pstr == num_font[i].c){
          pezled->ezledif->pbuff[index] = num_font[i].font;
          break;
        }
      index ++;
      if(index == pezled->ezledif->szbuff)
        break;
    }
    pstr++;
  }
  pezled->charlen = index;
  if(pezled->charlen > pezled->ezledif->count)
    pezled->private.scroll_en = 1;  //too many to dispaly, need to scroll
  else
    pezled->private.scroll_en = 0;
}

/**
 * @brief init the ezled instance with a led hardware interface.
 * @param pezled: the pointer to ezled instance
 * @param phardware: the led hardware pointer.
 * @return 0 if succeeded, otherwise negative number
*/
int8_t ezled_init(ezled_def* pezled, ezledif_def*phardware){
  if(pezled == 0) return -1;
  if(phardware == 0) return -1;
  
  pezled->ezledif = phardware;
  phardware->phook = pezled;
  if(pezled->ezledif->init)
    pezled->ezledif->init();  //init hardware.
  else return -2;
  pezled->blink_speed = LED_SPEED3;  //default settings for blinkspeed.
  pezled->scroll_speed = LED_SPEED4;  //default settings for scroll_speed.
  pezled->led_contrast = LEDCONT_LEVL3;   //default settings for contrast
  pezled->charlen = 0;
  pezled->private.curr_pos = 0;
  pezled->private.disp_en = 0xff;
  pezled->private.count_pre_div = 0;
  pezled->private.count_blink_div = 0;
  pezled->private.count_scroll_div = 0;
  pezled->private.scroll_en = 0;
  pezled->private.scroll_pos = 0;
  ezled_print(pezled, "HELLO");
  ezled_set_blink(pezled, 0x0);
  return 0;
}