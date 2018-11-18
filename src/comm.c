#include "comm.h"
#include "cbuff.h"
#include "usart.h"
#include "string.h"
#include "stdio.h"

#define COMM_LINE_BUFF_SIZE 256

CBuff_st comm_cbuff;
static char comm_line_buff[COMM_LINE_BUFF_SIZE];

static unsigned long comm_line_index = 0;
static comm_cmd_def cmd_code;
static unsigned long data_count;
	
void comm_init(void)
{
  usart_init();
	CBuff_Init(&comm_cbuff);
	memset(comm_line_buff,0,sizeof(comm_line_buff));
}

//void comm_send_data(comm_cmd_def cmd,unsigned long data_length, const void *_pdata)
//{
//	char *pdata = (char*)_pdata;
//	
//	printf("/s/%c",cmd);
//	uart_char(data_length&0xff);
//	uart_char((data_length>>8)&0xff);
//	uart_char((data_length>>16)&0xff);
//	uart_char((data_length>>24)&0xff);
//	
//	while(data_length--)
//	{
//		uart_char(*pdata);
//		if(*pdata == '/')
//		{
//			uart_char('a');
//		}
//		pdata++;
//	}
//	printf("/e");
//}

void comm_exe_cmd(void)
{
	unsigned char total_data_length;
	
	if(comm_line_index == 0)
	{
		//error
		//comm_puts("data not enough");
		return;
	}
	
	total_data_length = *(unsigned long*)comm_line_buff;
	if(data_count != total_data_length+4)
	{
		//error data length not right
		//comm_puts("data length not right");
		return;
	}
	
	switch(cmd_code)
	{
		case comm_cmd_test_connect:
			break;
		case comm_cmd_set_up_peripheral:
			break;
		case comm_cmd_write_peripheral:
			break;
		case comm_cmd_read_peripheral:
			break;
		default://error
			//comm_puts("cmd undefined");
			break;
	}
}

void comm_input_data(char c)
{	
	static char flag_next_char_is_cmd;
	static char flag_start_cmd_dected;
	static char flag_cmd_code_dected;
	
	if(comm_line_index == COMM_LINE_BUFF_SIZE)
	{
		//error occured
		comm_line_index = 0;
		flag_next_char_is_cmd = 0;
		//comm_puts("error:data overlap");
		return;
	}
	
	if(c =='/')
	{
		flag_next_char_is_cmd = 1;
		return;
	}
	
	if(flag_next_char_is_cmd)
	{//control cmd?
		flag_next_char_is_cmd = 0;
		
		if(c == 'a')
		{//not control cmd, this is data '/'
			data_count++;
			comm_line_buff[comm_line_index++] = '/';
			return;
		}
		else
		{
			//control cmd
			switch(c)
			{
				case 's'://start cmd
					flag_start_cmd_dected = 1;
					comm_line_index = 0;
					data_count = 0;
					break;
				case 'e'://end cmd, execute cmd
					if(flag_cmd_code_dected)
					{
						comm_line_buff[comm_line_index] = 0;
						comm_exe_cmd();
					}
					else
					{
						//error
						//comm_puts("cmd not dected");
					}
					flag_start_cmd_dected = 0;
					flag_cmd_code_dected = 0;
					comm_line_index = 0;
					data_count = 0;
					break;
				default: 
					if(flag_start_cmd_dected)
					{
						if(comm_line_index != 0)
						{
							//error
							flag_start_cmd_dected = 0;
							flag_cmd_code_dected = 0;
							comm_line_index = 0;
							data_count = 0;
							//comm_puts("cmd format error");
						}
						else
						{
							flag_cmd_code_dected = 1;
							cmd_code = (comm_cmd_def)c;
						}
					}
					else
					{
						//error
						//comm_puts("start cmd not detected");
					}
					break;
			}
		}
	}
	else
	{
		data_count++;
		comm_line_buff[comm_line_index++] = c;	
	}
	
}

void comm_service(void)
{
	char c;

	if(CBuff_Read(&comm_cbuff,&c) == CBuff_EOK)
	{
		comm_input_data(c);
	}
	
}
