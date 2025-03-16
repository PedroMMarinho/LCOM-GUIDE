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


