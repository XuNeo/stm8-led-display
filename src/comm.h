#ifndef _COMM_H_
#define _COMM_H_

typedef enum
{
	comm_ret_ok = 0,
	comm_ret_open_error,
	comm_ret_get_state_error,
	comm_ret_set_state_error,
	comm_ret_set_mask_error,
	comm_ret_set_timeout_error,

	comm_ret_write_data_error ,
	comm_ret_cmd_time_out ,
	comm_ret_unknown_adc_inf,
	comm_ret_unknown_dac_inf,
	comm_ret_data_length_error ,
	comm_ret_connect_test_error ,
	comm_ret_buff_not_enough,
	comm_ret_peripheral_cannot_write,
	comm_ret_peripheral_cannot_read,

	comm_ret_null_pointer,

	comm_ret_m360_not_return,

} comm_error_def;


typedef enum
{
	comm_cmd_test_connect = 0,
	comm_cmd_respond,
	comm_cmd_print,

	comm_cmd_set_up_peripheral,
	comm_cmd_write_peripheral,
	comm_cmd_read_peripheral,
	comm_cmd_send_progress,
} comm_cmd_def;


void comm_init(void);
void comm_service(void);
void comm_send_data(comm_cmd_def cmd,unsigned long data_length, const void *_pdata);

#endif
