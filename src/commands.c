#include "commands.h"
#include "serial_frame.h"
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
#define CMD_SETCONTRAST     3       //set the blink speed
#define CMD_PRINT           4       //set the blink speed

void command_set_blink(uint8_t *ppara, uint8_t len);
void command_print(uint8_t *ppara, uint8_t len);
void command_set_contrast(uint8_t *ppara, uint8_t len);

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
    .command = CMD_SETCONTRAST,
    .phandler = command_set_contrast,
    .pdesc = "",
  },
  {
    .command = CMD_PRINT,
    .phandler = command_print,
    .pdesc = "",
  },
};

sframe_def sframe;

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

static void _usart_rx_callback(uint8_t ch){
  sframe_decode(&sframe, &ch, 1);
}

//the function will be called when a valid frame is decoded.
static void _sframe_callback(uint8_t* pbuff, uint32_t len){
  command_parser(pbuff, len);
}

void commands_init(void){
  static uint8_t frame_buff[128];
  usart_init(9600, (void*)_usart_rx_callback);
  sframe_init(&sframe, frame_buff, 128, _sframe_callback);  //used to decode frame and store decoded frame to buffer.
}

void command_set_blink(uint8_t *ppara, uint8_t len){
  if(ppara == 0) return;
  if(len != 1) return;
  ezled_set_blink(&ezled, *ppara);
}

void command_set_contrast(uint8_t *ppara, uint8_t len){
  if(ppara == 0) return;
  if(len != 1) return;
  ezled_set_contrast(&ezled, (ledcont_def)*ppara);
}

void command_print(uint8_t *ppara, uint8_t len){
  //case string to len
  char *pstr = (char*)ppara;
  uint8_t str_len = 0;
  if(ppara == 0) return;
  while(*ppara++) str_len++;
  if(str_len > len)
    ppara[len] = '\0'; //something is wrong.
  ezled_print(&ezled, pstr);
}