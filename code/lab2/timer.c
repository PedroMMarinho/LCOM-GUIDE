#include <lcom/lcf.h>
#include <lcom/timer.h>

#include <stdint.h>

#include "i8254.h"


int hook_id = 0; // Timer hook_id needs to be unique
int timer_counter = 0; // Timer counter needs to be global


int (timer_set_frequency)(uint8_t timer, uint32_t freq) {

  if (freq < 19 || freq > TIMER_FREQ) {
    printf("Invalid frequency\n");
    return 1;
  }

  uint8_t st;
  if (timer_get_conf(timer, &st))
    return 1;
  // Create the control word
  uint8_t control_word = ((TIMER_SEL0 + timer) << 6) | ((st << 4) >> 4) | TIMER_LSB_MSB;

  // Write control word to the control register

  if(sys_outb(TIMER_CTRL, control_word))
    return 1;


  // Load the divisor to the timer register
  uint16_t divisor = TIMER_FREQ / freq;
  uint8_t lsb, msb;
  // Get the LSB and MSB of the divisor
  if(util_get_LSB(divisor, &lsb) || util_get_MSB(divisor, &msb))
      return 1;
  
  // Write the LSB and MSB to the timer register (ORDER MATTERS) 
  if(sys_outb(TIMER_0 + timer, lsb) || sys_outb(TIMER_0 + timer, msb))
        return 1;

  return 0;
}

int (timer_subscribe_int)(uint8_t *bit_no) {
  if(bit_no == NULL) return 1;
  *bit_no = BIT(hook_id); // This value will change after setting the policy
  if(sys_irqsetpolicy(TIMER0_IRQ, IRQ_REENABLE, &hook_id) != OK) // Set correct policy
    return 1;

  return 0;
}

int (timer_unsubscribe_int)() {
  if(sys_irqrmpolicy(&hook_id) != OK) // Remove the policy
    return 1;
  return 0;
}

void (timer_int_handler)() {
  timer_counter++;
}

int (timer_get_conf)(uint8_t timer, uint8_t *st) {

  uint8_t read_back_command = TIMER_RB_CMD | TIMER_RB_COUNT_ | TIMER_RB_SEL(timer);
  if (sys_outb(TIMER_CTRL, read_back_command))
    return 1;
  
  if(util_sys_inb(TIMER_0 + timer, st))
    return 1;

  return 0;
}

int (timer_display_conf)(uint8_t timer, uint8_t st,
                        enum timer_status_field field) {

  union timer_status_field_val status_field_val;

  switch (field) {
    case tsf_all:
      status_field_val.byte = st; // Byte is the same as the status
      break;
    case tsf_initial:
      status_field_val.in_mode = (st & TIMER_LSB_MSB) >> 4; // get the Initialization Mode
      break;
    case tsf_mode:
      status_field_val.count_mode = (st & TIMER_OPERATING_MODE) >> 1; // get the Operating Mode
      if(status_field_val.count_mode == 6 || status_field_val.count_mode == 7) status_field_val.count_mode -= 4;
      break;
    case tsf_base:
      status_field_val.bcd = st & TIMER_BCD; // get the Counting Base mode
      break;
    default:
      return 1;

  }
  
  if(timer_print_config(timer, field, status_field_val)) return 1;

  return 0;
}
