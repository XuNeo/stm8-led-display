#ifndef _PARAMETER_H_
#define _PARAMETER_H_
#include "ezled.h"

#define PARA_SIGNITURE 0x4c
typedef struct{
  uint8_t contrast[2][MAX_LED_NUM];
  led_speed_def blink_speed;
  led_speed_def scroll_speed;
  uint8_t signiture;
}ezled_para_def;

void parameter_load(ezled_def *ezled);
void parameter_set(ezled_para_def *ppara);
ezled_para_def* parameter_get(void);

#endif
