#include <lcom/lcf.h>

#include <lcom/lab3.h>

#include "i8042.h"
#include "keyboard.h"
#include <stdbool.h>
#include <stdint.h>

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

extern int kbd_error;
extern uint8_t scancode;
extern int sys_inb_cnt;

int(kbd_test_scan)() {
  int ipc_status;
  message msg;
  int r;
  uint8_t irq_set;
  bool esc_pressed = false;
  bool isTwoByteScanCode = false;

  if (kbd_subscribe_int(&irq_set))
    return 1;
  // Interrupt loop that runs until the ESC key is pressed
  while (!esc_pressed) {

    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & irq_set) {
            kbc_ih();

            if (!kbd_error) {

              // Check type of ScanCode
              if (scancode == ESC_BREAK) {
                esc_pressed = true;
              }
              if (scancode == INVALID_SCAN_CODE) {
                continue;
              }
              if (scancode == TWO_BYTE_CODE) {
                isTwoByteScanCode = true;
                continue;
              }

              uint8_t size = 1;
              uint8_t bytes[2];

              if (isTwoByteScanCode) {
                isTwoByteScanCode = false;
                size = 2;
              }
              // Create array of scancode(s)
              if (create_scancode_array(bytes, size))
                return 1;
              // Print to the terminal
              if (kbd_print_scancode(is_make_code(scancode), size, bytes))
                return 1;
            }
          }
          break;
        default:
          break;
      }
    }
    else {
    }
  }
  if (kbd_unsubscribe_int())
    return 1;
  if (kbd_print_no_sysinb(sys_inb_cnt))
    return 1;
  printf("Ended sucessfully\n");
  return 0;
}

int(kbd_test_poll)() {
  
  bool esc_pressed = false;
  bool isTwoByteScanCode = false;

  int tries = 0; // Optional to avoid infinite loop
  
  // Pool loop that runs until the ESC key is pressed
  while (!esc_pressed && tries < 250) {
    tries++;
    kbc_poll();

    if (!kbd_error) {
      // Check type of ScanCode
      if (scancode == ESC_BREAK) {
        esc_pressed = true;
      }
      if (scancode == INVALID_SCAN_CODE) {
        continue;
      }
      if (scancode == TWO_BYTE_CODE) {
        isTwoByteScanCode = true;
        continue;
      }

      uint8_t size = 1;
      uint8_t bytes[2];

      if (isTwoByteScanCode) {
        isTwoByteScanCode = false;
        size = 2;
      }

      // Create array of scancode(s)
      if (create_scancode_array(bytes, size))
        return 1;
      // Print to the terminal
      if (kbd_print_scancode(is_make_code(scancode), size, bytes))
        return 1;
    }
  }
  
  // Terminate the program correctly
  uint8_t arg_ret;
  if (loop_over_kbc_command(KBC_READ_CMD, &arg_ret))
    return 1;
  if (loop_over_kbc_command(KBC_WRITE_CMD, &arg_ret))
    return 1;
  if (kbd_print_no_sysinb(sys_inb_cnt))
    return 1;
  printf("Ended sucessfully\n");
  return 0;
}

extern int timer_counter;

int(kbd_test_timed_scan)(uint8_t idle) {
  int ipc_status;
  message msg;
  int r;
  uint8_t kbd_int_bit, timer0_int_bit;
  bool esc_pressed = false;
  bool isTwoByteScanCode = false;
  uint8_t seconds_passed = 0;

  	
  if (timer_subscribe_int(&timer0_int_bit))
    return 1;
  if (kbd_subscribe_int(&kbd_int_bit))
    return 1;
  // Interrupt loop that runs until the ESC key is pressed or idle has passed
  while (!esc_pressed && (seconds_passed != idle)) {

    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if(msg.m_notify.interrupts & timer0_int_bit){
            timer_int_handler();

            if(timer_counter % 60 == 0){
                seconds_passed++;
                printf("Seconds passed %d\n", seconds_passed);
            }
          }
          // Keyboard Interrupt
          if (msg.m_notify.interrupts & kbd_int_bit) {
            // Reset time passed
            seconds_passed = 0;
            kbc_ih();

            if (!kbd_error) {

              // Check type of ScanCode
              if (scancode == ESC_BREAK) {
                esc_pressed = true;
              }
              if (scancode == INVALID_SCAN_CODE) {
                continue;
              }
              if (scancode == TWO_BYTE_CODE) {
                isTwoByteScanCode = true;
                continue;
              }

              uint8_t size = 1;
              uint8_t bytes[2];

              if (isTwoByteScanCode) {
                isTwoByteScanCode = false;
                size = 2;
              }
              // Create array of scancode(s)
              if (create_scancode_array(bytes, size))
                return 1;
              // Print to the terminal
              if (kbd_print_scancode(is_make_code(scancode), size, bytes))
                return 1;
            }
          }
          break;
        default:
          break;
      }
    }
    else {
    }
  }
  if (kbd_unsubscribe_int())
    return 1;
  if (timer_unsubscribe_int()) 
    return 1;
  if (kbd_print_no_sysinb(sys_inb_cnt))
    return 1;
  printf("Ended sucessfully\n");
  return 0;
}
