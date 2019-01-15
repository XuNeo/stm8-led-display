#ifndef _FIFO_H_
#define _FIFO_H_
#include "stm8s.h"

typedef enum
{
	fifo_err_ok = 0,
	fifo_err_full = -1,
	fifo_err_empty = -2
}fifo_err_def;

typedef struct
{
  uint8_t *pbuff;
  uint32_t buff_size;
  uint32_t write_index;
  uint32_t read_index;
  uint32_t data_count;
}fifo_def;

void fifo_init(fifo_def *pfifo, uint8_t *pbuff, uint32_t len);
uint32_t fifo_status(fifo_def *pfifo);
fifo_err_def fifo_read(fifo_def *pfifo, uint8_t *pbuff, uint32_t *plen);
fifo_err_def fifo_write(fifo_def *pfifo, uint8_t *pbuff, uint32_t *plen);

#endif
