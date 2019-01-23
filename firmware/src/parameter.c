#include "parameter.h"
#include "eeprom.h"

/**
 * The parameters stored in eeprom for ezled.
*/
static ezled_para_def _ezled_para@0x4000;
const static ezled_para_def default_ezled_para={
  .signiture = PARA_SIGNITURE,
  .contrast = {{1,20,1,3,10,20,20,20},{1,1,1,1,1,3,3,3}},
  .blink_speed = LED_SPEED3,
  .scroll_speed = LED_SPEED7,
};

/**
 * @brief load default parameters to ezled.
*/
void parameter_load(ezled_def *ezled){
  uint8_t i;
  if(ezled == 0) return;
  if(_ezled_para.signiture != PARA_SIGNITURE){
    eeprom_write((uint8_t*)&_ezled_para, (uint8_t*)&default_ezled_para, sizeof(ezled_para_def));
  }
  ezled_set_blink_speed(ezled, _ezled_para.blink_speed);
  ezled_set_scroll_speed(ezled, _ezled_para.scroll_speed);
  for(i=0;i<MAX_LED_NUM;i++){
    ezled_set_contrastA(ezled, 1<<i, _ezled_para.contrast[0][i]);
    ezled_set_contrastB(ezled, 1<<i, _ezled_para.contrast[1][i]);
  }
}

void parameter_set(ezled_para_def *ppara){
  uint8_t i;
  /* compare with the parameters stored and only modify needed one. */
  if(ppara == 0)  return;
  if(ppara->blink_speed != _ezled_para.blink_speed){
    eeprom_write((uint8_t*)&_ezled_para.blink_speed, (uint8_t*)&ppara->blink_speed, 1);
  }
  for(i=0;i<MAX_LED_NUM;i++){
    if(ppara->contrast[0][i] != _ezled_para.contrast[0][i]){
      eeprom_write((uint8_t*)&_ezled_para.contrast[0][i], (uint8_t*)&ppara->contrast[0][i], 1);
    }
    if(ppara->contrast[1][i] != _ezled_para.contrast[1][i]){
      eeprom_write((uint8_t*)&_ezled_para.contrast[1][i], (uint8_t*)&ppara->contrast[1][i], 1);
    }
  }
  if(ppara->scroll_speed != _ezled_para.scroll_speed){
    eeprom_write((uint8_t*)&_ezled_para.scroll_speed, (uint8_t*)&ppara->scroll_speed, 1);
  }
}

ezled_para_def* parameter_get(void){
  return &_ezled_para;
}