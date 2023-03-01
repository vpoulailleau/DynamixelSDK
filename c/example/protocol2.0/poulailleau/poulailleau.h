#ifndef POULAILLEAU_H_
#define POULAILLEAU_H_

#include <stdint.h>
#include "dynamixel_sdk.h"

#define ESC_ASCII_VALUE 0x1b

#define MX28_LED_ON 1
#define MX28_LED_OFF 0
#define MX28_LED_SET(id, state) dxl_write_1byte_tx_rx(id, 25, state)

void log_info(char *msg);
void log_error(char *msg);

int getch(void);
int kbhit(void);

int dxl_open(char *serial_link, uint32_t baudrate);
void dxl_close(void);
void dxl_ping(int dynamixel_id);
void dxl_write_1byte_tx_rx(
    int dynamixel_id, int register_addr, uint8_t register_value);
void dxl_write_4byte_tx_rx(
    int dynamixel_id, int register_addr, uint32_t register_value);
uint32_t dxl_read_4byte_tx_rx(int dynamixel_id, int register_addr);

#endif /* POULAILLEAU_H_ */
