#include <lcom/lcf.h>
#include "i8042.h"

int kbc_error = 1;
uint8_t byte;

void (set_kbc_error)(int err){
    kbc_error = err;
}

int (read_KBC_status_reg)(uint8_t *st){
    if(st == NULL) return 1;
    if(util_sys_inb(KBC_ST_REG,st)) return 1;
    return 0;
}

int (check_KBC_status)(){
    uint8_t status;
    if(read_KBC_status_reg(&status)) return 1;
    return status & KBC_IBF;
}

int (issue_kbc_command)(uint8_t cmd, uint8_t *arg_ret_value){
    if (arg_ret_value == NULL) return 1;

    // Commands must be written in address 0x64
    // Args need to be passed using address 0x60
    // Return values can be read in the out_buf(0x60) register
    switch (cmd)
    {
    case KBC_READ_CMD:
        // Check if IBF is set (Input buffer is full) 
        if(check_KBC_status()) return 1;
        if(sys_outb(KBC_CMD_REG,cmd)) return 1;
        if(util_sys_inb(KBC_OUT_BUF,arg_ret_value)) return 1;
        break;
    case KBC_WRITE_CMD:
        if(check_KBC_status()) return 1;
        if(sys_outb(KBC_CMD_REG,cmd)) return 1;
        if(check_KBC_status()) return 1;
        if(sys_outb(KBC_CMD_ARG_REG,*arg_ret_value)) return 1;
        break;
    case KBC_WRITE_MOUSE:
        if(check_KBC_status()) return 1;
        if(sys_outb(KBC_CMD_REG,cmd)) return 1;
        if(check_KBC_status()) return 1;
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

int (read_kbc_state)(int ismouse){
    kbc_error = 1;
    uint8_t status;
    // Read status register
    if(util_sys_inb(KBC_ST_REG, &status)) return 1;

    bool cannot_read_out_buf = !(status & KBC_OBF); // If 0 then can read

    if (ismouse) 
        cannot_read_out_buf = !(status & KBC_AUX) || cannot_read_out_buf;
    else 
        cannot_read_out_buf = (status & KBC_AUX) || cannot_read_out_buf;

    kbc_error = status & (KBC_PARITY | KBC_TIMEOUT) || cannot_read_out_buf;
    
    // Check if OBF is set (Data available for reading)
    return cannot_read_out_buf;
}

int (get_byte)(uint8_t *bt){
    if (bt == NULL) return 1;
    *bt = byte;
    return 0;
}

int (read_out_buf)(){
    if(util_sys_inb(KBC_OUT_BUF, &byte)) return 1;
    return 0;
}

int (get_kbc_error)(int *kbc_err){
    if(kbc_err == NULL) return 1;
    *kbc_err = kbc_error;
    return 0;
}
