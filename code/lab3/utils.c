#include <lcom/lcf.h>

#include <stdint.h>

int sys_inb_cnt = 0; // Counter for sys_inb calls

int (util_get_LSB)(uint16_t val, uint8_t *lsb) {
  if(lsb == NULL) return 1;
  *lsb = val;
  return 0;
}

int (util_get_MSB)(uint16_t val, uint8_t *msb) {
  if(msb == NULL) return 1;
  *msb = val >> 8;
  return 0;
}

// Reads the value of a 8-bit register from a given port
int (util_sys_inb)(int port, uint8_t *value) {
  if (value == NULL) return 1;
  uint32_t val;

  int ret = sys_inb(port, &val);
  #ifdef LAB3
  sys_inb_cnt++;
  #endif
  
  *value = val;
  return ret;
}

// Writes a value to a 8-bit register from a given port
int (util_sys_outb)(int port, uint8_t *command) {
  if (command == NULL) return 1;
  int ret = sys_outb(port, *command);
  return ret;
}

