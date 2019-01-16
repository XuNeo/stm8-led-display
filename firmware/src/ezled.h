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
  LEDCONT_LEVL0 = 0,  //lowest contrast
  LEDCONT_LEVL1,
  LEDCONT_LEVL2,
  LEDCONT_LEVL3,
  LEDCONT_LEVL4,
  LEDCONT_LEVL5,
  LEDCONT_LEVL6,
  LEDCONT_LEVL7,
  LEDCONT_LEVL8,
  LEDCONT_LEVL9,      //highest contrast
}ledcont_def; //led contrast level

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
  struct _ezled * phook;                /* The hook to ezled instance which is bounded to this hardware. */
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
  uint8_t             flag_interrupt; /**< flag to indicate the timer interrupt has occured. */
  ezledif_def         *ezledif;       /* The hardware interface */
  uint8_t             charlen;        /* Character length not including '.' */
  uint8_t             blink_pos_set;  /* Private variables */
  led_speed_def       blink_speed;    /* Blink speed. */
  led_speed_def       scroll_speed;
  ledcont_def         led_contrast;
  struct{ //private variables
    uint8_t curr_pos;
    uint8_t disp_en;        //used for blink. 
    uint8_t count_pre_div;  //the counter for pre-divider.
    uint8_t count_blink_div;    
    uint8_t count_scroll_div;    
    uint8_t scroll_en;      //enable scroll function(when content to display exceed led count.).
    uint8_t scroll_pos;     //curr start position to disp buffer(ezledif.pbuff).
  }private;
}ezled_def;

int8_t ezled_init(ezled_def* pezled, ezledif_def*phardware);
void ez_led_poll(ezled_def* pezled);
void ezled_print(ezled_def* pezled, char *pstr);
void ezled_set_blink(ezled_def *pezled, uint8_t pos_set);
void ezled_set_blink_speed(ezled_def *pezled, led_speed_def speed);
void ezled_set_scroll_speed(ezled_def *pezled, led_speed_def speed);
void ezled_set_contrast(ezled_def *pezled, ledcont_def contrast);
void ezled_timer_isr(ezled_def *pezled);

#endif
