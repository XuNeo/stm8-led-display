#ifndef _LED_H_
#define _LED_H_

typedef struct
{
  void (*led_init)(void);
}led_bsp_def;

void led_init(void);
void led_print(char *pstr);


#endif