#include <lcom/lcf.h>

#include "i8042.h"
#include <stdint.h>
#include "kbc.h"

int hook_id_mouse = 2;
struct packet pp;

int (mouse_subscribe_int)(uint8_t *bit_no){
    if (bit_no == NULL) return 1;
    *bit_no = BIT(hook_id_mouse);
    if(sys_irqsetpolicy(MOUSE_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id_mouse)) return 1;
    return 0;
}

int (mouse_unsubscribe_int)(){
    if(sys_irqrmpolicy(&hook_id_mouse)) return 1;
    return 0;
}

int (get_packet)(struct packet *packet){
    if (packet == NULL) return 1;
    *packet = pp;
    return 0;
}

int (handle_mouse_command)(uint8_t *cmd){
    if(cmd == NULL) return 1;
    
    uint8_t max_tries = MAX_TRIES;
    uint8_t ack_byte = NACK;
    
    while (max_tries--)
    {
        if(loop_over_kbc_command(MOUSE_WRITE_BYTE,cmd)) return 1;

        if(tickdelay(micros_to_ticks(DELAY_US))) return 1;
        // READ OUT_BUF ACK byte
        if (util_sys_inb(KBC_OUT_BUF,&ack_byte)) return 1;
        
        if (ack_byte == ACK) return 0;
    }
    return 1;
}

int (mouse_enable_data_report)(){
    uint8_t cmd = ENABLE_DATA_REPORTING;
    if(handle_mouse_command(&cmd)) return 1;
    return 0;
}

int (mouse_disable_data_report)(){
    uint8_t cmd = DISABLE_DATA_REPORTING;
    if(handle_mouse_command(&cmd)) return 1;
    return 0;
}

void (mouse_ih)(){
    if(read_kbc_state(1)) return;
    if(read_out_buf()) return;
    set_kbc_error(0);
}


void (parse_packet)(){
    pp.lb = pp.bytes[0] & BIT(0);
    pp.rb = pp.bytes[0] & BIT(1);
    pp.mb = pp.bytes[0] & BIT(2);
    pp.delta_x = (pp.bytes[0] & BIT(4)) ? pp.bytes[1] | 0xFF00 : pp.bytes[1];
    pp.delta_y = (pp.bytes[0] & BIT(5)) ? pp.bytes[2] | 0xFF00 : pp.bytes[2];
    pp.x_ov = pp.bytes[0] & BIT(6);
    pp.y_ov = pp.bytes[0] & BIT(7);
}

int (handle_byte_sinc)(uint8_t *byte_index){
    if (byte_index == NULL) return 1;
    uint8_t current_byte;
    if (get_byte(&current_byte)) return 1;

    if ((current_byte & BIT(3)) && *byte_index == 0){
        *byte_index = 1;
        pp.bytes[0] = current_byte;
        return 0;
    } else if (*byte_index > 0){
        pp.bytes[*byte_index] = current_byte;
        *byte_index = (*byte_index + 1);
        return 0;
    }
    return 1;
}
