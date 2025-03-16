#ifndef _LCOM_I8042_H_
#define _LCOM_I8042_H_

#include <lcom/lcf.h>



/** @defgroup i8042 i8042
 * @{
 *
 * Constants for programming the i8042 KBC. Needs to be completed.
*/

#define KBD_IRQ 1 /**< @brief Keyboard IRQ line */

/* KBC Registers */ 
#define KBC_ST_REG 0x64
#define KBC_CMD_REG 0x64
#define KBC_OUT_BUF 0x60

/* KBC Status Register*/

#define KBC_PARITY BIT(7)
#define KBC_TIMEOUT BIT(6)
#define KBC_AUX BIT(5)
#define KBC_INH BIT(4)
#define KBC_A2 BIT(3)
#define KBC_SYS BIT(2)
#define KBC_IBF BIT(1)
#define KBC_OBF BIT(0)

/* Scancodes */

#define ESC_BREAK 0x81
#define TWO_BYTE_CODE 0xE0
#define INVALID_SCAN_CODE 0x00
#define IS_BREAK_CODE BIT(7)

#endif /* _LCOM_I8042_H */
