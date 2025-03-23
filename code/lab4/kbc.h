#ifndef __KBC_H
#define __KBC_H

#include <lcom/lcf.h>

int (read_KBC_status_reg)(uint8_t *st);
int (check_KBC_status)();
int (issue_kbc_command)(uint8_t cmd, uint8_t *arg_ret_value);
int (loop_over_kbc_command)(uint8_t cmd, uint8_t *arg_ret_value);
int (read_out_buf)();
int (get_kbc_error)(int *kbc_err);
int (read_kbc_state)(int ismouse);
void (set_kbc_error)(int err);
int (get_byte)(uint8_t *byte);


#endif
