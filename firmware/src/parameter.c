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
#include "parameter.h"
#include "eeprom.h"

/**
 * The parameters stored in eeprom for ezled.
*/
static ezled_para_def _ezled_para@0x4080;
/**
 * Default parameters stored in flash.
*/
const static ezled_para_def default_ezled_para={
  .signiture = PARA_SIGNITURE,
  .contrast = {{90,90,90,90,90,90,90,90},{70,30,9,3,1,3,3,3}, {1,3,9,20,70,1,1,1}},
  .blink_speed = LED_SPEED0,
  .scroll_speed = LED_SPEED0,
  .addr = 0,  //do not use address 0, which is the broadcast address.
};

/**
 * @brief load default parameters to ezled.
 * @param ezled: ezled instance pointer.
*/
void parameter_load(ezled_def *ezled){
  uint8_t i;
  if(ezled == 0) return;
  if(_ezled_para.signiture != PARA_SIGNITURE){
    ezled_para_def shadow;
    uint8_t addr = 0;
    for(int i=0;i<12;i++)
      addr ^= *(uint8_t*)(0x4865+i);  //init the address with unique id.
    //copy default settings to shadow var.
    char *pdst = (char*)&shadow;
    char *psrc = (char*)&default_ezled_para;
    for(int i=0;i<sizeof(ezled_para_def);i++){
      *pdst++ = *psrc++;
    }
    shadow.addr = addr;
    eeprom_write((uint8_t*)&_ezled_para, (uint8_t*)&shadow, sizeof(ezled_para_def));
  }
  ezled_set_blink_speed(ezled, _ezled_para.blink_speed);
  ezled_set_scroll_speed(ezled, _ezled_para.scroll_speed);
  for(i=0;i<MAX_LED_NUM;i++){
    ezled_set_contrastA(ezled, 1<<i, _ezled_para.contrast[0][i]);
    ezled_set_contrastB(ezled, 1<<i, _ezled_para.contrast[1][i]);
  }
  ezled_set_contrastC(ezled, _ezled_para.contrast[2]);
}

/**
 * @brief store changed parameters to eeprom
 * @param ppara: the pointer to parameter.
*/
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
    if(ppara->contrast[2][i] != _ezled_para.contrast[2][i]){
      eeprom_write((uint8_t*)&_ezled_para.contrast[2][i], (uint8_t*)&ppara->contrast[2][i], 1);
    }
  }
  if(ppara->scroll_speed != _ezled_para.scroll_speed){
    eeprom_write((uint8_t*)&_ezled_para.scroll_speed, (uint8_t*)&ppara->scroll_speed, 1);
  }
  if(ppara->addr != _ezled_para.addr){
    eeprom_write((uint8_t*)&_ezled_para.addr, (uint8_t*)&ppara->addr, 1);
  }
}

ezled_para_def* parameter_get(void){
  return &_ezled_para;
}