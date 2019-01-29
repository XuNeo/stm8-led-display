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
  {'G', LEDSEGC|LEDSEGD|LEDSEGA|LEDSEGB|LEDSEGF|LEDSEGG,          },/* G */
  {'h', LEDSEGF|LEDSEGE|LEDSEGG|LEDSEGC,                          },/* h */
  {'H', LEDSEGC|LEDSEGE|LEDSEGB|LEDSEGF|LEDSEGG,                  },/* H */
  {'l', LEDSEGE|LEDSEGF,                                          },/* l */
  {'L', LEDSEGD|LEDSEGE|LEDSEGF,                                  },/* L */
  {'N', LEDSEGC|LEDSEGE|LEDSEGB|LEDSEGF|LEDSEGA,                  },/* N */
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

static const uint8_t led_font_count = sizeof(num_font)/sizeof(led_font_def);

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
  if(pezled->private.hlight_en)
    contrast = pezled->private.contrast[pezled->private.curr_pos];
  else
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
 * @brief set which one led to highlight.
 * @param whichled: must be in range of ezled->ezledif->count - 1 starts from 0
*/
void ezled_set_hlight(ezled_def *ezled, uint8_t whichled){
  uint8_t i, index;
  if(ezled == 0) return;
  if(whichled > ezled->ezledif->count - 1){
    ezled->private.hlight_en = 0;
    return;
  }
  ezled->private.hlight_en = 1;
  index = ezled->ezledif->count - 1; //get the maximum contrast value index.
  for(i=0; i<=whichled; i++)
    ezled->private.contrast[i] = ezled->contrast[2][index - whichled + i];
  for(;i<ezled->ezledif->count; i++)
    ezled->private.contrast[i] = ezled->contrast[2][index - (i - whichled)];
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
 * @brief set led contrast group C. Group C is used for high-light function.
 * This table defines the contrast value with the first one has lowest contrast and the end one(
 * or the last one that used by this pezled) has highest contrast.
 * @param contrast[8]: The contrast table, where only pezled->ezledif->count number of element is needed.
 * @return none.
*/
void ezled_set_contrastC(ezled_def *pezled,  uint8_t contrast[8]){
  uint8_t i=0;
  if(pezled == 0) return;
  if(contrast == 0) return;
  for(;i<8;i++){
    pezled->contrast[2][i] = contrast[i];
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
  if(pezled->charlen > pezled->ezledif->count){
    pezled->private.scroll_en = 1;  //too many to dispaly, need to scroll
	pezled->private.scroll_pos = 0; //start from beginning
  }
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
  uint8_t i, temp;
  char str[9];
  if(pezled == 0) return -1;
  if(phardware == 0) return -1;
  
  pezled->ezledif = phardware;
  if(pezled->ezledif->init)
    pezled->ezledif->init();  //init hardware.
  else return -2;
  
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
  i=0;
  str[i++] = 'A';
  str[i++] = 'D';
  if(pezled->ezledif->count > 4)
    str[i++] = 'D';
  str[i++] = '.';
  temp = pezled->ezledif->address>>4;
  if(temp<0xa) temp = '0' + temp;
  else temp = 'A' + temp-0xa;
  str[i++] = temp;
  temp = pezled->ezledif->address&0xf;
  if(temp<0xa) temp = '0' + temp;
  else temp = 'A' + temp-0xa;
  str[i++] = temp;
  str[i] = 0;
  ezled_print(pezled, str);
  ezled_set_blink(pezled, 0);
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
