#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#include <stdbool.h>
#include <stdint.h>


int (kbd_subscribe_int)(uint8_t *bit_no);
int (kbd_unsubscribe_int)();
int (create_scancode_array)(uint8_t byte[], uint8_t size);
bool (is_make_code)(uint8_t scancode);
int (loop_over_kbc_command)(uint8_t cmd, uint8_t *arg_ret_value);
void (kbc_poll)();
#endif
