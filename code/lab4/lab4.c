// IMPORTANT: you must include the following line in all your C files
#include <lcom/lcf.h>

#include <stdint.h>
#include <stdio.h>
#include "mouse.h"
# include "kbc.h"
#include "i8042.h"
// Any header files included below this line should have been created by you

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need/ it]
  lcf_trace_calls("/home/lcom/labs/lab4/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab4/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}


int (mouse_test_packet)(uint32_t cnt) {

  int ipc_status;
  message msg;
  int r;

  uint8_t mouse_int_bit;
  uint8_t num_byte = 0;
  printf("Started\n");
  // Set the mouse to stream mode
  // if (mouse_enable_data_reporting()) return 1; // Provided function
  if (mouse_enable_data_report()) return 1; // Our function 
  
  if (mouse_subscribe_int(&mouse_int_bit))
    return 1;
  

  
  // Read cnt packets
  while (cnt) {

    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:
          if (msg.m_notify.interrupts & mouse_int_bit) {
             mouse_ih();
             // Check if there was an error
             int error;
             if (get_kbc_error(&error))
               return 1;
              // If no errors were found
              if (!error){

              if(handle_byte_sinc(&num_byte)) return 1; 

              if (num_byte == 3){

                parse_packet();
                struct packet pp;
                if (get_packet(&pp))
                  return 1;
                mouse_print_packet(&pp);
                num_byte = 0;
                cnt--;
              }

            }

          }
          break;
        default:
          break;
      }
    }
  }
  

  
  if (mouse_unsubscribe_int())
    return 1;

  if (mouse_disable_data_report())
    return 1;

  printf("Ended sucessfully\n");
  return 0;
}

extern int timer_counter;

int (mouse_test_async)(uint8_t idle_time) {
  int ipc_status;
  message msg;
  int r;
  int frequency = sys_hz();
  uint8_t mouse_int_bit;
  uint8_t timer_int_bit;

  uint8_t num_byte = 0;
  uint8_t time_passed = 0;
  printf("Started\n");
  // Set the mouse to stream mode
  // if (mouse_enable_data_reporting()) return 1; // Provided function
  if (mouse_enable_data_report()) return 1; // Our function 
  
  if (mouse_subscribe_int(&mouse_int_bit))
    return 1;

  if (timer_subscribe_int(&timer_int_bit))
    return 1;
  
  // Read cnt packets
  while (time_passed < idle_time) {

    if ((r = driver_receive(ANY, &msg, &ipc_status)) != 0) {
      printf("driver_receive failed with: %d", r);
      continue;
    }
    if (is_ipc_notify(ipc_status)) {
      switch (_ENDPOINT_P(msg.m_source)) {
        case HARDWARE:

          if (msg.m_notify.interrupts & timer_int_bit) {
            timer_int_handler();
            if (timer_counter % frequency == 0){
              time_passed++;
            }
          }
          if (msg.m_notify.interrupts & mouse_int_bit) {
             time_passed = 0;
             timer_counter = 0;
             mouse_ih();
             // Check if there was an error
             int error;
             if (get_kbc_error(&error))
               return 1;
              // If no errors were found
              if (!error){

              if(handle_byte_sinc(&num_byte)) return 1; 

              if (num_byte == 3){

                parse_packet();
                struct packet pp;
                if (get_packet(&pp))
                  return 1;
                mouse_print_packet(&pp);
                num_byte = 0;
              }

            }

          }
          break;
        default:
          break;
      }
    }
  }
  
  
  if (mouse_unsubscribe_int())
    return 1;

  if (mouse_disable_data_report())
    return 1;

  printf("Ended sucessfully\n");
  return 0;
}

int (mouse_test_gesture)(uint8_t x_len, uint8_t tolerance) {
    /* To be completed */
    printf("%s: under construction\n", __func__);
    return 1;
}

int (mouse_test_remote)(uint16_t period, uint8_t cnt) {
    /* This year you need not implement this. */
    printf("%s(%u, %u): under construction\n", __func__, period, cnt);
    return 1;
}
