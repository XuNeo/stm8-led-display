#ifndef _COMMANDS_H_
#define _COMMANDS_H_
#include "stm8s.h"
typedef struct{
  uint8_t command;
  void (*phandler)(uint8_t *, uint8_t);
  char *pdesc;        //description of this command.
}cmd_table_def;

void commands_init(void);
void commands_poll(void);

#endif
