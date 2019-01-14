#ifndef _EZLED_H_
#define _EZLED_H_
#include "stm8s.h"

#define LEDSEGA     0x01
#define LEDSEGB     0x02
#define LEDSEGC     0x04
#define LEDSEGD     0x08
#define LEDSEGE     0x10
#define LEDSEGF     0x20
#define LEDSEGG     0x40
#define LEDSEGDP    0x80

typedef enum
{
  LEDPOS1 = 0x01,
  LEDPOS2 = 0x02,
  LEDPOS3 = 0x04,
  LEDPOS4 = 0x08,
  LEDPOS5 = 0x10,
  LEDPOS6 = 0x20,
  LEDPOS7 = 0x40,
  LEDPOS8 = 0x80,
  LEDPOSNON,
}ledpos_def;

typedef enum
{
  LEDCONT_LEVL0 = 0,
  LEDCONT_LEVL1,
  LEDCONT_LEVL2,
  LEDCONT_LEVL3,
  LEDCONT_LEVL4,
  LEDCONT_LEVL5,
  LEDCONT_LEVL6,
  LEDCONT_LEVL7,
  LEDCONT_LEVL8,
  LEDCONT_LEVL9,
}ledcont_def; //led contrast level

typedef enum
{
  LEDBLINK_SPEED0 = 0,
  LEDBLINK_SPEED1,
  LEDBLINK_SPEED2,
  LEDBLINK_SPEED3,
  LEDBLINK_SPEED4,
  LEDBLINK_SPEED5,
  LEDBLINK_SPEED6,
  LEDBLINK_SPEED7,
  LEDBLINK_SPEED8,
  LEDBLINK_SPEED9,
}ledblink_speed_def;

/* ezLedLib interface definination*/

typedef struct
{
  uint32_t  led_pos_count;                /* How many numbers can display on led screen */
  
  void      (*led_bsp_init)       (void); /* Init LED related hardwares */
  void      (*led_light_up)       (uint8_t position, uint8_t seg_set);
  uint32_t  (*get_led_segs)       (void); /* Return how many segs are connected to this LED interface */
  void      (*set_timer_callback) (void *pfunc);  /* Call this function in timer isr. */
}ezledif_def;

typedef struct
{
  uint8_t c;
  uint8_t font;
}led_font_def;

typedef struct
{
  uint8_t *const pled_seg_buff;       /* The raw data to display on LED directly */
  ezledif_def ezledif;                /* The hardware interface */
  uint8_t blink_pos_set;  /* Private variables */
  ledblink_speed_def blink_speed;
  ledcont_def led_contrast;
  struct
  {
    uint8_t curr_pos;
    uint8_t disp_en;    //used for blink. 
    uint8_t blink_count;  
    uint8_t led_font_count;       /* How many characters are in table. */
    const led_font_def *pled_font;      /* The font used to translate string to raw seg data.  */
    const uint8_t *pcontrast_table;
  }private;
}ezled_def;

extern ezled_def ezled;

void ezled_init(ezled_def* pezled);
void ezled_print(ezled_def* pezled, char *pstr);
void ezled_set_blink(ezled_def *pezled, uint8_t pos_set);
void ezled_set_blink_speed(ezled_def *pezled, ledblink_speed_def speed);
void ezled_set_contrast(ezled_def *pezled, ledcont_def contrast);
void ezled_timer_isr(ezled_def *pezled);
void ez_led_poll(ezled_def* pezled);

#endif
