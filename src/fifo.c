#include "fifo.h"
#include "string.h"

void fifo_init(fifo_def *pfifo, uint8_t *pbuff, uint32_t len)
{
  pfifo->pbuff = pbuff;
  pfifo->buff_size = len;
  pfifo->write_index = 0;
  pfifo->read_index = 0;
  pfifo->data_count = 0;
}

uint32_t fifo_status(fifo_def *pfifo)
{
  return pfifo->data_count;
}

fifo_err_def fifo_write(fifo_def *pfifo, uint8_t *pbuff, uint32_t *plen){
  uint32_t write_len = *plen;
  uint32_t temp;
  if(pfifo->data_count >= pfifo->buff_size)
    return fifo_err_full;
  /* step1, how many data we can write? */
  write_len = write_len > pfifo->buff_size-pfifo->data_count?\
              pfifo->buff_size-pfifo->data_count : write_len;
  /* step2, how many data can be written directly to the buffer. */
  temp = pfifo->buff_size - pfifo->write_index; //we write from 'write_index' to end of buffer.
  if(write_len < temp){ //good, we can write all data at once.
    memcpy(pfifo->pbuff+pfifo->write_index, pbuff, write_len);
    /* !modify critical pointers. */
    pfifo->write_index += write_len;
    pfifo->data_count += write_len;
    /* exit critical area. */
  }
  else{ //we need two steps to write.
    //firstly, write data until to end of buffer.
    memcpy(pfifo->pbuff+pfifo->write_index, pbuff, temp);
    //then write from start of buffer.
    if(write_len != temp)
      memcpy(pfifo->pbuff, pbuff+temp, write_len-temp);
    /* !modify critical pointers. */
    pfifo->write_index = write_len-temp;
    pfifo->data_count += write_len;
    /* exit critical area. */
  }
  *plen = write_len;
  return fifo_err_ok;
}

fifo_err_def fifo_read(fifo_def *pfifo, uint8_t *pbuff, uint32_t *plen)
{
  uint32_t read_len, temp;
  if(pfifo->data_count == 0)
    return fifo_err_empty;
  read_len = pfifo->data_count > *plen?\
              *plen: pfifo->data_count;
  /* How many data we can read directly from buffer? */
  temp = pfifo->buff_size - pfifo->read_index;
  if(read_len < temp){  //good, we can read directly.
    memcpy(pbuff, pfifo->pbuff+pfifo->read_index, read_len);
    //enter critical area.
    pfifo->read_index += read_len;
    pfifo->data_count -= read_len;
    //exit critical area.
  }
  else{//we need two steps to read all data.
    memcpy(pbuff, pfifo->pbuff + pfifo->read_index, temp);
    //then read from buffer start.
    if(read_len != temp)
      memcpy(pbuff+temp, pfifo->pbuff, read_len-temp);
    /* enter critical area */
    pfifo->read_index = read_len - temp;
    pfifo->data_count -= read_len;
    //exit critical area.
  }
  *plen = read_len;
  return fifo_err_ok;
}


