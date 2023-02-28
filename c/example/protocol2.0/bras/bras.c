
#include <stdlib.h>
#include <stdio.h>
#include <poulailleau.h>

// Control table address
#define ADDR_PRO_TORQUE_ENABLE 562
#define ADDR_PRO_GOAL_POSITION 596
#define ADDR_PRO_PRESENT_POSITION 611

#define DXL_ID 1 // Dynamixel ID: 1
#define BAUDRATE 57600
#define DEVICENAME "/dev/ttyUSB0" // Check which port is being used on your controller
                                  // ex) Windows: "COM1"   Linux: "/dev/ttyUSB0" Mac: "/dev/tty.usbserial-*"

#define TORQUE_ENABLE 1
#define TORQUE_DISABLE 0
#define DXL_MINIMUM_POSITION_VALUE -150000
#define DXL_MAXIMUM_POSITION_VALUE 150000
#define DXL_MOVING_STATUS_THRESHOLD 20

int main()
{
    int index = 0;
    int dxl_goal_position[2] = {
        DXL_MINIMUM_POSITION_VALUE, DXL_MAXIMUM_POSITION_VALUE};
    int32_t dxl_present_position = 0;

    dxl_open(DEVICENAME, BAUDRATE);

    dxl_write_1byte_tx_rx(DXL_ID, ADDR_PRO_TORQUE_ENABLE, TORQUE_ENABLE);

    while (1)
    {
        printf("Press any key to continue! (or press ESC to quit!)\n");
        if (getch() == ESC_ASCII_VALUE)
            break;

        dxl_write_4byte_tx_rx(
            DXL_ID, ADDR_PRO_GOAL_POSITION, dxl_goal_position[index]);

        do
        {
            dxl_present_position = dxl_read_4byte_tx_rx(
                DXL_ID, ADDR_PRO_PRESENT_POSITION);
            printf(
                "[ID:%03d] GoalPos:%03d  PresPos:%03d\n",
                DXL_ID,
                dxl_goal_position[index],
                dxl_present_position);
        } while (
            abs(dxl_goal_position[index] - dxl_present_position) > DXL_MOVING_STATUS_THRESHOLD);

        // Change goal position
        index = 1 - index;
    }

    dxl_write_1byte_tx_rx(DXL_ID, ADDR_PRO_TORQUE_ENABLE, TORQUE_DISABLE);

    dxl_close();

    return 0;
}
