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
  {'n', LEDSEGE|LEDSEGG|LEDSEGC,                                  },/* n */
  {'o', LEDSEGC|LEDSEGD|LEDSEGE|LEDSEGG,                          },/* o */
  {'O', LEDSEGC|LEDSEGD|LEDSEGE|LEDSEGA|LEDSEGB|LEDSEGF,          },/* O */
  {'p', LEDSEGE|LEDSEGA|LEDSEGB|LEDSEGG|LEDSEGF,                  },/* P */
  {'r', LEDSEGE|LEDSEGG,                                          },/* r */
  {'s', LEDSEGC|LEDSEGD|LEDSEGA|LEDSEGF|LEDSEGG,                  },/* s */
  {'t', LEDSEGD|LEDSEGE|LEDSEGF|LEDSEGG,                          },/* t */
  {'u', LEDSEGC|LEDSEGD|LEDSEGE,                                  },/* u */
  {'y', LEDSEGC|LEDSEGD|LEDSEGB|LEDSEGF|LEDSEGG,                  },/* y */
  {'-', LEDSEGG,                                                  },/* - */
  {' ', 0,                                                        },/* LED Dark */
};

static uint8_t led_font_count = sizeof(num_font)/sizeof(led_font_def);

/**
 * @brief ezled main loop. call this function periodically.
 * @return none.
*/
void ezled_poll(ezled_def* pezled){
  uint8_t pos2mask, contrast;
  //contrast divider
  pezled->private.count_contrast++;
  if(pezled->private.count_contrast == 101)
    pezled->private.count_contrast = 0;
  //scan led counter.
  pezled->private.curr_pos ++;
  if(pezled->private.curr_pos == pezled->ezledif->count)
    pezled->private.curr_pos = 0;

  pos2mask = 1<<pezled->private.curr_pos;
  contrast = pezled->contrast[(pos2mask&pezled->private.contrast_sel)?1:0][pezled->private.curr_pos];

  if(pezled->private.count_contrast < contrast){ //this led should be displayed.
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
      pezled->ezledif->light(pos2mask, led);
  }
  else{//shut down all
    pezled->ezledif->light(LEDPOSNON, 0);
  }
  /* Do blink */
  pezled->private.count_pre_div++;
  if(pezled->private.count_pre_div == 100){
    pezled->private.count_pre_div = 0;
    if(pezled->blink_pos_set){ //not empty
      pezled->private.count_blink_div++;
      if(pezled->private.count_blink_div > (LED_SPEED9 +1 - pezled->blink_speed)*20){
        pezled->private.count_blink_div = 0;
        pezled->private.contrast_sel ^= pezled->blink_pos_set;
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
 * @brief set which seg to blink.
 * @param pos_set: bit-set to select which seg to blink.\
 * @return none.
*/
void ezled_set_blink(ezled_def *pezled, uint8_t pos_set){
  if(pezled == 0) return;
  pezled->blink_pos_set = pos_set;
  pezled->private.contrast_sel = 0;
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
 * @brief set led contrast group A. Group A is used for normal display. When blink is
 *        enabled, the contrast will blink between Group A and B.
 * @param contrast: 0 to 100. 0 means totally off.
 * @return none.
*/
void ezled_set_contrastA(ezled_def *pezled, uint8_t pos_set, uint8_t contrast){
  uint8_t i=0;
  if(pezled == 0) return;
  for(;i<8;i++){
    if(pos_set&0x01)  pezled->contrast[0][i] = contrast; //set all selected led contrast.
    pos_set >>= 1;
  }
}

/**
 * @brief set led contrast group A. Group A is used for normal display. When blink is
 *        enabled, the contrast will blink between Group A and B.
 * @param contrast: 0 to 100. 0 means totally off.
 * @return none.
*/
void ezled_set_contrastB(ezled_def *pezled, uint8_t pos_set, uint8_t contrast){
  uint8_t i=0;
  if(pezled == 0) return;
  //pezled->contrast[] = contrast;
  for(;i<8;i++){
    if(pos_set&0x01)  pezled->contrast[1][i] = contrast;
    pos_set >>= 1;
  }
}

/**
 * @brief print string to led. String will be truncated by number of led.
 * @param pstr: ponter to string.
*/
void ezled_print(ezled_def* pezled, char *pstr){
  uint8_t index = 0;
  uint8_t flag_found;
  pezled->charlen = 0;
  while(*pstr){
    uint8_t i;
    flag_found = 0;
    if(*pstr == '.'){
      if(index)
        pezled->ezledif->pbuff[index-1] |= LEDSEGDP;
    }
    else{
      pezled->ezledif->pbuff[index] = 0;
      if(pezled->fontcount){ //firstly search if there is customized font.
        for(i=0;i<pezled->fontcount;i++){
          if(*pstr == pezled->pfontbuf[i].c){
            pezled->ezledif->pbuff[index] = pezled->pfontbuf[i].font;
            flag_found = 1;
            break;
          }
        }
      }
      if(flag_found == 0){  //continue to search in build-in font
        for(i=0;i<led_font_count;i++)
          if(*pstr == num_font[i].c){
            pezled->ezledif->pbuff[index] = num_font[i].font;
            break;
        }
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
  uint8_t i;
  if(pezled == 0) return -1;
  if(phardware == 0) return -1;
  
  pezled->ezledif = phardware;
  if(pezled->ezledif->init)
    pezled->ezledif->init();  //init hardware.
  else return -2;
  pezled->blink_speed = LED_SPEED4;  //default settings for blinkspeed.
  pezled->scroll_speed = LED_SPEED0;  //default settings for scroll_speed.
  //init contrast table;
  for(i=0;i<MAX_LED_NUM;i++){
    pezled->contrast[0][i] = 50;  //50%
    pezled->contrast[1][i] = 0;   //3% - the minimum ON contrast, use 0 to turn off it.
  }
  pezled->charlen = 0;
  pezled->fontcount = 0;
  pezled->pfontbuf = 0;
  pezled->buffsz = 0;
  pezled->private.curr_pos = 0;
  pezled->private.count_pre_div = 0;
  pezled->private.count_blink_div = 0;
  pezled->private.count_scroll_div = 0;
  pezled->private.scroll_en = 0;
  pezled->private.scroll_pos = 0;
  pezled->private.contrast_sel = 0;
  ezled_print(pezled, "HELLO");
  ezled_set_blink(pezled, LEDPOS2);
  return 0;
}

/**
 * @brief set the font buffer.
*/
void ezled_set_fontbuf(ezled_def* pezled, uint8_t *pbuff, uint32_t buffsz){
  if(pezled == 0) return;
  if(buffsz == 0) return;
  if(pbuff == 0) return;
  pezled->fontcount = 0;
  pezled->pfontbuf = (led_font_def*)pbuff;
  pezled->buffsz = buffsz/sizeof(led_font_def);
}

int8_t ezled_font_append(ezled_def* pezled, const led_font_def *pchar){
  if(pezled == 0) return -1;
  if(pchar == 0) return -1;
  if(pezled->fontcount == pezled->buffsz) return -2;  //each font needs two bytes.
  pezled->pfontbuf[pezled->fontcount++] = *pchar;
  return 0;
}
