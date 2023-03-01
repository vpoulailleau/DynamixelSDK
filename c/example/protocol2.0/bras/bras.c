
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <poulailleau.h>

// Control table address
#define ADDR_PRO_TORQUE_ENABLE 562
#define ADDR_PRO_GOAL_POSITION 596
#define ADDR_PRO_PRESENT_POSITION 611

#define SHOULDER_1_ID 1
#define SHOULDER_2_ID 2
#define ELBOW_ID 3
#define DXL_ID 1 // TODO à virer

#define BAUDRATE 57600
#define DEVICENAME "COM1" // Check which port is being used on your controller
                          // Windows: "COM1"
                          // Linux: "/dev/ttyUSB0"
                          // Mac: "/dev/tty.usbserial-*"

#define TORQUE_ENABLE 1
#define TORQUE_DISABLE 0
#define DXL_MINIMUM_POSITION_VALUE -150000
#define DXL_MAXIMUM_POSITION_VALUE 150000
#define DXL_MOVING_STATUS_THRESHOLD 20

float conversion_position_degre(float position)
{
    return position / 4096.0 * 360;
}

uint32_t conversion_position_degre(float position)
{
    return (uint32_t)(position * 4096 / 360.0);
}

void info_moteur(int dynamixel_id)
{
    // TODO
}

int main()
{
    int index = 0;
    int dxl_goal_position[2] = {
        DXL_MINIMUM_POSITION_VALUE, DXL_MAXIMUM_POSITION_VALUE};
    int32_t dxl_present_position = 0;

    log_info("Ouverture de la liaison série (COM1)");
    dxl_open(DEVICENAME, BAUDRATE);

    log_info("Connexion aux moteurs");
    dxl_ping(SHOULDER_1_ID);
    dxl_ping(SHOULDER_2_ID);
    dxl_ping(ELBOW_ID);

    log_info("Allumage des LED des moteurs");
    MX28_LED_SET(SHOULDER_1_ID, MX28_LED_ON);
    MX28_LED_SET(SHOULDER_2_ID, MX28_LED_ON);
    MX28_LED_SET(ELBOW_ID, MX28_LED_ON);

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
