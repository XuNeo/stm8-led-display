#include "ezled.h"
#include "stm8s.h"

const led_font_def num_font[]=
{
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
  {'b', LEDSEGC|LEDSEGD|LEDSEGE|LEDSEGF|LEDSEGG,                  },/* b */
  {'c', LEDSEGD|LEDSEGE|LEDSEGF|LEDSEGA,                          },/* c */
  {'d', LEDSEGC|LEDSEGD|LEDSEGE|LEDSEGB|LEDSEGG,                  },/* d */
  {'e', LEDSEGD|LEDSEGE|LEDSEGA|LEDSEGB|LEDSEGF|LEDSEGG           },/* e */
  {'E', LEDSEGD|LEDSEGE|LEDSEGA|LEDSEGF|LEDSEGG,                  },/* e */
  {'f', LEDSEGE|LEDSEGA|LEDSEGF|LEDSEGG,                          },/* f */
  {'g', LEDSEGC|LEDSEGD|LEDSEGA|LEDSEGB|LEDSEGF|LEDSEGG,          },/* g */
  {'-', LEDSEGG,                                                  },/* - */
  {'h', LEDSEGC|LEDSEGE|LEDSEGB|LEDSEGF|LEDSEGG,                  },/* H */
  {'o', LEDSEGC|LEDSEGD|LEDSEGE|LEDSEGG,                          },/* o */
  {'l', LEDSEGD|LEDSEGE|LEDSEGF,                                  },/* L */
  {'p', LEDSEGE|LEDSEGA|LEDSEGB|LEDSEGG|LEDSEGF,                  },/* P */
  {' ', 0,                                                        },/* LED Dark */
};

const uint8_t contrast_table[10]=
{
  200,100,50,25,15,10,8,6,4,0,
};

void ezled_set_blink(ezled_def *pezled, uint8_t pos_set)
{
  if(pezled == 0) return;
  pezled->blink_pos_set = pos_set;
  pezled->private.disp_en = 0xff;
}
void ezled_set_blink_speed(ezled_def *pezled, ledblink_speed_def speed)
{
  if(pezled == 0) return;
  pezled->blink_speed = speed;
}
void ezled_set_constrast(ezled_def *pezled, ledcont_def contrast)
{
  if(pezled == 0) return;
  pezled->led_contrast = contrast;
}

void ezled_timer_isr(ezled_def *pezled)
{
  static uint8_t blink_div_count;
  if(pezled->private.curr_pos < pezled->ezledif.led_pos_count)
  {
    uint8_t pos2index = 1<<pezled->private.curr_pos;
    if(pos2index & pezled->private.disp_en) //if this led is allow to display.
      pezled->ezledif.led_light_up(pos2index, pezled->pled_seg_buff[pezled->private.curr_pos]);
    else
      pezled->ezledif.led_light_up(LEDPOSNON, 0);
  }
  else
  {//shut down all
    pezled->ezledif.led_light_up(LEDPOSNON, 0);
  }
  pezled->private.curr_pos ++;
  if(pezled->private.curr_pos > pezled->private.pcontrast_table[pezled->led_contrast] + pezled->ezledif.led_pos_count)
    pezled->private.curr_pos = 0;
  /* Do blink */
  blink_div_count++;
  if(blink_div_count == 50)
  {
    blink_div_count = 0;
    if(pezled->blink_pos_set) //not empty
    {
      pezled->private.blink_count++;
      if(pezled->private.blink_count > (LEDBLINK_SPEED9 - pezled->blink_speed)*20)
      {
        pezled->private.blink_count = 0;
        pezled->private.disp_en ^= pezled->blink_pos_set;
      }
    }
  }
}

void ezled_print(ezled_def* pezled, char *pstr)
{
//pled_seg_buff
  uint8_t index = 0;
  while(*pstr)
  {
    uint8_t i;
    if(*pstr == '.')
    {
      if(index)
        pezled->pled_seg_buff[index-1] |= LEDSEGDP;
    }
    else
    {
      pezled->pled_seg_buff[index] = 0;
      for(i=0;i<pezled->private.led_font_count;i++)
        if(*pstr == pezled->private.pled_font[i].c)
        {
          pezled->pled_seg_buff[index] = pezled->private.pled_font[i].font;
          break;
        }
      index ++;
      if(index > pezled->ezledif.led_pos_count)
        return;
    }
    pstr++;
  }
}

void ezled_init(ezled_def* pezled)
{
  pezled->ezledif.led_bsp_init();
  pezled->blink_speed = LEDBLINK_SPEED5;
  pezled->led_contrast = LEDCONT_LEVL9;
  pezled->private.pled_font = num_font;
  pezled->private.pcontrast_table = contrast_table;
  pezled->private.led_font_count = sizeof(num_font)/sizeof(led_font_def);
  ezled_print(pezled, "hEl.0");
  ezled_set_blink(pezled, 0);
}

