
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <poulailleau.h>

#define SHOULDER_1_ID 1
#define SHOULDER_2_ID 2
#define ELBOW_ID 3

#define BAUDRATE 1000000
#define DEVICENAME "COM1" // Check which port is being used on your controller
                          // Windows: "COM1"
                          // Linux: "/dev/ttyUSB0"
                          // Mac: "/dev/tty.usbserial-*"

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
    char msg[1024];
    log_debug("------------------------------------------------------");
    snprintf(msg, 1023, "Identifiant du moteur : %d", MX28_ID_GET(dynamixel_id));
    log_debug(msg);
    snprintf(msg, 1023, "État de la LED (0 éteinte, 1 allumée) : %d", MX28_LED_GET(dynamixel_id));
    log_debug(msg);
    snprintf(msg, 1023, "Gain P : %d", MX28_P_GAIN_GET(dynamixel_id));
    log_debug(msg);
    snprintf(msg, 1023, "Gain I : %d", MX28_I_GAIN_GET(dynamixel_id));
    log_debug(msg);
    snprintf(msg, 1023, "Gain D : %d", MX28_D_GAIN_GET(dynamixel_id));
    log_debug(msg);
    snprintf(
        msg,
        1023,
        "Angle horaire limite : %f° (%d)",
        conversion_position_degre(MX28_CW_ANGLE_LIMIT_GET(dynamixel_id)),
        MX28_CW_ANGLE_LIMIT_GET(dynamixel_id));
    log_debug(msg);
    snprintf(
        msg,
        1023,
        "Angle anti-horaire limite : %f° (%d)",
        conversion_position_degre(MX28_CCW_ANGLE_LIMIT_GET(dynamixel_id)),
        MX28_CCW_ANGLE_LIMIT_GET(dynamixel_id));
    log_debug(msg);
    snprintf(
        msg,
        1023,
        "Vitesse de déplacement : %f°/s (%d)",
        MX28_MOVING_SPEED_GET(dynamixel_id) * 6 * 0.114,
        MX28_MOVING_SPEED_GET(dynamixel_id));
    log_debug(msg);
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
