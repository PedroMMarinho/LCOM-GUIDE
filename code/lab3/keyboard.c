#include <lcom/lcf.h>

#include <i8042.h>
#include <stdint.h>


int hook_id_kbd = 1;
int kbd_error = 1;
uint8_t scancode;

int (kbd_subscribe_int)(uint8_t *bit_no){
    if (bit_no == NULL) return 1;
    *bit_no = BIT(hook_id_kbd);
    if(sys_irqsetpolicy(KBD_IRQ, IRQ_REENABLE | IRQ_EXCLUSIVE, &hook_id_kbd)) return 1;
    return 0;
}

int (kbd_unsubscribe_int)(){
    if(sys_irqrmpolicy(&hook_id_kbd)) return 1;
    return 0;
}

int (read_kbc_state)(){
    kbd_error = 0;
    uint8_t status;
    // Read status register
    if(util_sys_inb(KBC_ST_REG, &status)) return 1;
    
    kbd_error = status & (KBC_PARITY | KBC_TIMEOUT | KBC_AUX);
    // Check if OBF is set (Data available for reading)
    return !(status & KBC_OBF);
}

int (read_out_buf)(){
    if(util_sys_inb(KBC_OUT_BUF, &scancode)) return 1;
    return 0;
}


void (kbc_ih)(){
    if(read_kbc_state()) return;
    if(read_out_buf()) return;
    kbd_error = 0;
}

int (create_scancode_array)(uint8_t byte[], uint8_t size){
    if (byte == NULL) return 1;
    if (size > 2) return 1;
    
    byte[0] = scancode;
    
    if (size == 2){
        byte[0] = TWO_BYTE_CODE;
        byte[1] = scancode;
    } 
    return 0;
}

bool (is_make_code)(uint8_t scancode){
    return !(scancode & IS_BREAK_CODE);
}


int (issue_kbc_command)(uint8_t cmd, uint8_t *arg_ret_value){
    if (arg_ret_value == NULL) return 1;
    uint8_t status;
    if(util_sys_inb(KBC_ST_REG, &status)) return 1; // Read status register
    if(status & KBC_IBF) return 1; // Check if IBF is set (Input buffer is full)

    // Commands must be written in address 0x64
    // Args need to be passed using address 0x60
    // Return values can be read in the out_buf(0x60) register
    switch (cmd)
    {
    case KBC_READ_CMD:
        if(sys_outb(KBC_CMD_REG,cmd)) return 1;
        if(util_sys_inb(KBC_OUT_BUF,arg_ret_value)) return 1;
        break;
    case KBC_WRITE_CMD:
        if(sys_outb(KBC_CMD_REG,cmd)) return 1;
        if(sys_outb(KBC_CMD_ARG_REG,*arg_ret_value)) return 1;
        break;
    // Other commands can be handled here
    default:
        break;
    }

    return 0;
}


int (loop_over_kbc_command)(uint8_t cmd, uint8_t *arg_ret_value){
    if(arg_ret_value == NULL) return 1;
    
    uint8_t max_tries = MAX_TRIES;
    while (max_tries--)
    {
        if(issue_kbc_command(cmd,arg_ret_value) == 0){
            return 0;
        }
        if(tickdelay(micros_to_ticks(DELAY_US))) return 1;
    }
    return 1;
}
