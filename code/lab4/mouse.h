#ifndef __MOUSE_H
#define __MOUSE_H
#include <stdbool.h>
#include <stdint.h>

int (mouse_subscribe_int)(uint8_t *bit_no);
int (mouse_unsubscribe_int)();
int (issue_kbc_command)(uint8_t cmd, uint8_t *arg_ret_value);
int (loop_over_kbc_command)(uint8_t cmd, uint8_t *arg_ret_value);
int (mouse_enable_data_report)();
int (mouse_disable_data_report)();
void (mouse_ih)();
int (handle_byte_sinc)(uint8_t *num_byte);
int (get_packet)(struct packet *pp);
void (parse_packet)();

#endif

