#ifndef _SERIAL_FRAME_H_
#define _SERIAL_FRAME_H_
#include "stm8s.h"

#define SFRAME_START  0x7d
#define SFRAME_STOP   0x7d
#define SFRAME_ESCAPE 0x7e

typedef enum{
  sframe_state_start = 0,
  sframe_state_framelen,
  sframe_state_payload,
  sframe_state_escaping,
  sframe_state_crc,
  sframe_state_end,
}sframe_state_def;

typedef struct _sframe{
  uint8_t *pbuff;
  uint32_t max_frame_len;
  uint32_t frame_len;
  uint32_t windex;
  sframe_state_def state;
  void (*callback)(uint8_t *pframe, uint32_t len);
}sframe_def;

#endif
