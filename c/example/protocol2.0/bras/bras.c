
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

uint32_t conversion_degre_position(float position)
{
    return (uint32_t)(position * 4096 / 360.0);
}

void info_moteur(int dynamixel_id)
{
    // TODO
}

int main()
{
    char msg[1024];

    log_info("Ouverture de la liaison série (COM1)");
    dxl_open(DEVICENAME, BAUDRATE);

    log_info("Connexion aux moteurs");
    dxl_ping(SHOULDER_1_ID);
    dxl_ping(SHOULDER_2_ID);
    dxl_ping(ELBOW_ID);

    log_info("Allumage des LED des moteurs");
    MX28_LED_SET(SHOULDER_1_ID, MX28_ON);
    MX28_LED_SET(SHOULDER_2_ID, MX28_ON);
    MX28_LED_SET(ELBOW_ID, MX28_ON);

    /*
    info_moteur(SHOULDER_1_ID);
    info_moteur(SHOULDER_2_ID);
    info_moteur(ELBOW_ID);
    */

    log_info("Mise en route des moteurs");
    MX28_TORQUE_SET(SHOULDER_1_ID, MX28_ON);
    MX28_TORQUE_SET(SHOULDER_2_ID, MX28_ON);
    MX28_TORQUE_SET(ELBOW_ID, MX28_ON);

    log_info("Mise en position initiale");
    MX28_GOAL_POSITION_SET(SHOULDER_1_ID, conversion_degre_position(180));
    MX28_GOAL_POSITION_SET(SHOULDER_2_ID, conversion_degre_position(220));
    MX28_GOAL_POSITION_SET(ELBOW_ID, conversion_degre_position(260));
    sleep(1);
    MX28_MOVING_SPEED_SET(SHOULDER_1_ID, 40);
    MX28_MOVING_SPEED_SET(SHOULDER_2_ID, 40);
    MX28_MOVING_SPEED_SET(ELBOW_ID, 40);

    log_info("Déplacement de l'épaule");
    MX28_GOAL_POSITION_SET(SHOULDER_1_ID, 1500);
    MX28_GOAL_POSITION_SET(SHOULDER_2_ID, conversion_degre_position(180));

    for (uint8_t i = 0; i < 4; i++)
    {
        log_info("Déplacement du coude");
        MX28_GOAL_POSITION_SET(ELBOW_ID, conversion_degre_position(130));
        for (uint8_t j = 0; j < 20; j++)
        {
            sleep_ms(100);
            snprintf(
                msg,
                1023,
                "Position coude : %f°",
                conversion_position_degre(MX28_PRESENT_POSITION_GET(ELBOW_ID)));
            log_info(msg);
        }

        log_info("Déplacement du coude");
        MX28_GOAL_POSITION_SET(ELBOW_ID, conversion_degre_position(220));
        for (uint8_t j = 0; j < 20; j++)
        {
            sleep_ms(100);
            snprintf(
                msg,
                1023,
                "Position coude : %f°",
                conversion_position_degre(MX28_PRESENT_POSITION_GET(ELBOW_ID)));
            log_info(msg);
        }
    }

    log_info("Extinction des moteurs");
    MX28_GOAL_POSITION_SET(ELBOW_ID, conversion_degre_position(260));
    sleep(3);
    MX28_TORQUE_SET(SHOULDER_1_ID, MX28_OFF);
    MX28_TORQUE_SET(SHOULDER_2_ID, MX28_OFF);
    MX28_TORQUE_SET(ELBOW_ID, MX28_OFF);

    log_info("Extinction des LED des moteurs");
    MX28_LED_SET(SHOULDER_1_ID, MX28_ON);
    MX28_LED_SET(SHOULDER_2_ID, MX28_ON);
    MX28_LED_SET(ELBOW_ID, MX28_ON);

    MX28_MOVING_SPEED_SET(SHOULDER_1_ID, 0);
    MX28_MOVING_SPEED_SET(SHOULDER_2_ID, 0);
    MX28_MOVING_SPEED_SET(ELBOW_ID, 0);

    dxl_close();

    return 0;
}
