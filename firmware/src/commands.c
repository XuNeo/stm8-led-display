#include "commands.h"
#include "serial_frame.h"
#include "fifo.h"
#include "usart.h"
#include "ezled.h"
/**
 * Receive characters from UART and process it in sframe. The exacted frame
 * is then processed with commands.c
*/
/**
 * The frame data definition for a valid commands
 * BYTE0: command
 * BYTE1: parameter count in bytes
 * BYTE[2:BYTE1+1] parameters
*/

#define CMD_INVALID         0       //reserved command
#define CMD_HELLO           0xfe    //dummy command
#define CMD_SYS             0xff    //reserved command

#define CMD_SETBLINK        1       //start which led(s) to blink
#define CMD_SETBLINK_SPEED  2       //set the blink speed
#define CMD_SETCONTRASTA    3       //set the contrast level
#define CMD_SETCONTRASTB    4       //set the contrast level
#define CMD_PRINT           5       //print string to led.
#define CMD_SETSCROLL_SPEED 6       //set scroll speed
#define CMD_SAVE_SETTING    7       //save current settings as default settings.
#define CMD_ADD_FONT        8       //add temp font.

void command_set_blink(uint8_t *ppara, uint8_t len);
void command_set_blink_speed(uint8_t *ppara, uint8_t len);
void command_set_scroll_speed(uint8_t *ppara, uint8_t len);
void command_print(uint8_t *ppara, uint8_t len);
void command_set_contrastA(uint8_t *ppara, uint8_t len);
void command_set_contrastB(uint8_t *ppara, uint8_t len);
void command_save_settings(uint8_t *ppara, uint8_t len);
void command_add_font(uint8_t *ppara, uint8_t len);

cmd_table_def cmd_table[]={
  {
    .command = CMD_HELLO,
    .phandler = 0,
    .pdesc = "command hello",
  },
  {
    .command = CMD_SYS,
    .phandler = 0,
    .pdesc = "reserved command",
  },
  {
    .command = CMD_SETBLINK,
    .phandler = command_set_blink,
    .pdesc = "",
  },
  {
    .command = CMD_SETBLINK_SPEED,
    .phandler = command_set_blink_speed,
    .pdesc = "",
  },
  {
    .command = CMD_SETSCROLL_SPEED,
    .phandler = command_set_scroll_speed,
    .pdesc = "",
  },
  {
    .command = CMD_SETCONTRASTA,
    .phandler = command_set_contrastA,
    .pdesc = "",
  },
  {
    .command = CMD_SETCONTRASTB,
    .phandler = command_set_contrastB,
    .pdesc = "",
  },
  {
    .command = CMD_PRINT,
    .phandler = command_print,
    .pdesc = "",
  },
  {
    .command = CMD_SAVE_SETTING,
    .phandler = command_save_settings,
    .pdesc = "",
  },
  {
    .command = CMD_ADD_FONT,
    .phandler = command_add_font,
    .pdesc = "",
  },
  
};

sframe_def sframe;
fifo_def uart_fifo;
static inline void command_parser(uint8_t *pframe, uint32_t len){
  uint8_t cmd_code, para_len;
  uint8_t i;
  if(pframe == 0) return;
  if(len < 2) return; //we have cmd+len at least
  cmd_code = *pframe++;
  para_len = *pframe++;
  for(i=0; i<(sizeof(cmd_table)/sizeof(cmd_table_def)); i++){
    if(cmd_code == cmd_table[i].command){
      //found it. call it.
      if(cmd_table[i].phandler)
        cmd_table[i].phandler(pframe, para_len);
      break;
    }
  }
}

//add fifo support on usart receive side.
static void _usart_rx_callback(uint8_t ch){
  fifo_write1B(&uart_fifo, ch);
}

//the function will be called when a valid frame is decoded.
static void _sframe_callback(uint8_t* pbuff, uint32_t len){
  command_parser(pbuff, len);
}

void commands_init(void){
  static uint8_t frame_buff[128];
  static uint8_t fifobuff[128];
  fifo_init(&uart_fifo, fifobuff, 128);
  usart_init(115200, (void*)_usart_rx_callback);
  sframe_init(&sframe, frame_buff, 128, _sframe_callback);  //used to decode frame and store decoded frame to buffer.
}

//poll this functio to process command received(frame received)
void commands_poll(void){
  uint8_t ch;
  while(fifo_read1B(&uart_fifo, &ch) == fifo_err_ok)
    sframe_decode(&sframe, &ch, 1);
}
