#include "parameter.h"
#include "eeprom.h"

/**
 * The parameters stored in eeprom for ezled.
*/
static ezled_para_def _ezled_para@0x4000 ={
  .signiture = PARA_SIGNITURE,
  .contrast = LEDCONT_LEVL6,
  .blink_speed = LED_SPEED3,
  .scroll_speed = LED_SPEED7,
};

const static ezled_para_def default_ezled_para={
  .signiture = PARA_SIGNITURE,
  .contrast = LEDCONT_LEVL6,
  .blink_speed = LED_SPEED3,
  .scroll_speed = LED_SPEED7,
};

/**
 * @brief load default parameters to ezled.
*/
void parameter_load(ezled_def *ezled){
  if(ezled == 0) return;
  if(_ezled_para.signiture != PARA_SIGNITURE){
    eeprom_write((uint8_t*)&_ezled_para, (uint8_t*)&default_ezled_para, sizeof(ezled_para_def));
  }
  ezled_set_blink_speed(ezled, _ezled_para.blink_speed);
  ezled_set_scroll_speed(ezled, _ezled_para.scroll_speed);
  ezled_set_contrast(ezled, _ezled_para.contrast);
}

void parameter_set(ezled_para_def *ppara){
  /* compare with the parameters stored and only modify needed one. */
  if(ppara == 0)  return;
  if(ppara->blink_speed != _ezled_para.blink_speed){
    eeprom_write((uint8_t*)&_ezled_para.blink_speed, (uint8_t*)&ppara->blink_speed, 1);
  }
  if(ppara->contrast != _ezled_para.contrast){
    eeprom_write((uint8_t*)&_ezled_para.contrast, (uint8_t*)&ppara->contrast, 1);
  }
  if(ppara->scroll_speed != _ezled_para.scroll_speed){
    eeprom_write((uint8_t*)&_ezled_para.scroll_speed, (uint8_t*)&ppara->scroll_speed, 1);
  }
}

ezled_para_def* parameter_get(void){
  return &_ezled_para;
}