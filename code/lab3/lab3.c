#include <lcom/lcf.h>

#include <lcom/lab3.h>

#include <stdbool.h>
#include <stdint.h>
#include "keyboard.h"
#include "i8042.h"

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

  if(kbd_subscribe_int(&irq_set)) return 1;
  // Interrupt loop that runs until the ESC key is pressed
  while( !esc_pressed ) { 
     
     if ( (r = driver_receive(ANY, &msg, &ipc_status)) != 0 ) { 
         printf("driver_receive failed with: %d", r);
         continue;
     }
     if (is_ipc_notify(ipc_status)) { 
         switch (_ENDPOINT_P(msg.m_source)) {
             case HARDWARE: 			
                if (msg.m_notify.interrupts & irq_set) {
                    kbc_ih();

                    if (!kbd_error){
                        
                        // Check type of ScanCode
                        if(scancode == ESC_BREAK){
                          esc_pressed = true;
                        }
                        if(scancode == INVALID_SCAN_CODE){
                          continue;
                        }
                        if (scancode == TWO_BYTE_CODE){
                           isTwoByteScanCode = true;
                           continue;
                        }
                        
                        uint8_t size = 1;
                        uint8_t bytes[2];

                        if (isTwoByteScanCode){
                            isTwoByteScanCode = false;
                            size = 2;
                        }
                        // Create array of scancode(s)
                        if(create_scancode_array(bytes, size)) return 1;
                        // Print to the terminal
                        if(kbd_print_scancode(is_make_code(scancode),size,bytes)) return 1;

                    }
                 }
                 break;
             default:
                 break; 
         }
    } else { 
         
     }
  }
  if(kbd_unsubscribe_int()) return 1;
  if(kbd_print_no_sysinb(sys_inb_cnt)) return 1;
  printf("Ended sucessfully\n");
  return 0;
}

int(kbd_test_poll)() {
  /* To be completed by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int(kbd_test_timed_scan)(uint8_t n) {
  /* To be completed by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}
