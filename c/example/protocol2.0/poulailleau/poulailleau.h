#ifndef POULAILLEAU_H_
#define POULAILLEAU_H_

#include <stdint.h>
#include "dynamixel_sdk.h"

#define ESC_ASCII_VALUE 0x1b

#define MX28_ON 1
#define MX28_OFF 0
#define MX28_LED_SET(id, state) dxl_write_1byte_tx_rx(id, 25, state)
#define MX28_LED_GET(id) dxl_read_1byte_tx_rx(id, 25)
#define MX28_TORQUE_SET(id, state) dxl_write_1byte_tx_rx(id, 24, state)
#define MX28_GOAL_POSITION_SET(id, value) dxl_write_2byte_tx_rx(id, 30, value)
#define MX28_MOVING_SPEED_SET(id, value) dxl_write_2byte_tx_rx(id, 32, value)
#define MX28_MOVING_SPEED_GET(id) dxl_read_2byte_tx_rx(id, 32)
#define MX28_PRESENT_POSITION_GET(id) dxl_read_2byte_tx_rx(id, 36)
#define MX28_ID_GET(id) dxl_read_1byte_tx_rx(id, 3)
#define MX28_P_GAIN_GET(id) dxl_read_1byte_tx_rx(id, 28)
#define MX28_I_GAIN_GET(id) dxl_read_1byte_tx_rx(id, 27)
#define MX28_D_GAIN_GET(id) dxl_read_1byte_tx_rx(id, 26)
#define MX28_CW_ANGLE_LIMIT_GET(id) dxl_read_2byte_tx_rx(id, 6)
#define MX28_CCW_ANGLE_LIMIT_GET(id) dxl_read_2byte_tx_rx(id, 8)

void log_debug(char *msg);
void log_info(char *msg);
void log_error(char *msg);

int getch(void);
int kbhit(void);
unsigned int sleep(unsigned int nb_sec);
void sleep_ms(uint32_t ms);

int dxl_open(char *serial_link, uint32_t baudrate);
void dxl_close(void);
void dxl_ping(int dynamixel_id);
void dxl_write_1byte_tx_rx(
    int dynamixel_id, int register_addr, uint8_t register_value);
uint8_t dxl_read_1byte_tx_rx(int dynamixel_id, int register_addr);
void dxl_write_2byte_tx_rx(
    int dynamixel_id, int register_addr, uint16_t register_value);
uint16_t dxl_read_2byte_tx_rx(int dynamixel_id, int register_addr);
void dxl_write_4byte_tx_rx(
    int dynamixel_id, int register_addr, uint32_t register_value);
uint32_t dxl_read_4byte_tx_rx(int dynamixel_id, int register_addr);

#endif /* POULAILLEAU_H_ */
