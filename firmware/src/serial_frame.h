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
#ifndef _SERIAL_FRAME_H_
#define _SERIAL_FRAME_H_
#include "stm8s.h"

#define SFRAME_START  0x7d
#define SFRAME_STOP   0x7c  //i don't want to use same mark as start and stop.
#define SFRAME_ESCAPE 0x7e

typedef enum{
  sframe_state_start = 0,
  sframe_state_framelen,
  sframe_state_payload,
  sframe_state_escaping,
  sframe_state_crc,
  sframe_state_end,
}sframe_state_def;

typedef void (*sframe_callback)(uint8_t*, uint32_t);
typedef void (*sframe_outfunc)(uint8_t);

/**
 * @brief this structure is only used for decoder.
 * The encoder will send out data when encoding.
*/
typedef struct _sframe{
  uint8_t *pbuff;             /**< buffer used to store decoded frame. */
  uint32_t buffsz;            /**< buffer size */
  uint32_t frame_len;         /**< decoded frame length. */
  uint32_t windex;            /**< the index to current buff position */
  sframe_state_def state;     /**< current state. */
  sframe_callback callback;   /**< the function that will be called if a frame is successfully decode. */
}sframe_def;

void sframe_init(sframe_def *psframe, uint8_t *pbuff, uint32_t buffsz, sframe_callback callback);
int32_t sframe_decode(sframe_def *psframe, uint8_t *pinput, uint32_t len);
int32_t sframe_encode(sframe_outfunc pfunc, uint8_t *pdata, uint32_t len);

#endif
