/**
 * Serial frame uses the similar method in HDLC protocol.
 * high level data link control protocol
 * 0x7d as frame start and end marker
 * 0x7e is the escape uint8_tacter, both data 0x7d and 0x7e should be
 * escaped with 0x7e + data^0x20.
 * So, data 0x7e will be encoded to 0x7e+0x5e; data 0x7d will be 
 * encoded to 0x7e+0x5d
 * */
#include "serial_frame.h"

void sframe_init(sframe_def *psframe, uint8_t *pbuff, uint32_t buffer_size){
  if(psframe == 0) return;
  psframe->pbuff = pbuff;
  psframe->max_frame_len = buffer_size;
  psframe->state = sframe_state_start;
  psframe->frame_len = 0;
  psframe->windex = 0;
}

int32_t sframe_decode(sframe_def *psframe, uint8_t *pinput, uint32_t len){
  while(len--){
    switch(psframe->state){
      case sframe_state_start:  //waiting for start flag.
        if(*pinput == SFRAME_START)
          psframe->state = sframe_state_framelen; //next byte is frame length
        break;
      case sframe_state_framelen:  //frame length
        psframe->frame_len = *pinput;
        psframe->state = sframe_state_payload;
        break;
      case sframe_state_payload:
        if(*pinput == SFRAME_ESCAPE)
          psframe->state = sframe_state_escaping; //this is not a data.
        else
          psframe->pbuff[psframe->windex++] = *pinput;
        if(psframe->windex == psframe->frame_len) 
          //we got enough data, check the end flag.
          psframe->state = sframe_state_end;
        break;
      case sframe_state_escaping: //this is an escaping data.
        psframe->pbuff[psframe->windex++] = *pinput ^ 0x20;
        psframe->state = sframe_state_payload;
        if(psframe->windex == psframe->frame_len) 
          //we got enough data, check the end flag.
          psframe->state = sframe_state_end;
        break;
      case sframe_state_end:
        if(*pinput == SFRAME_STOP){
          if(psframe->callback)
            psframe->callback(psframe->pbuff, psframe->frame_len);
          psframe->state = sframe_state_start;
        }
        break;
    }
    pinput ++;
  }
  return 0;
}

