
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <poulailleau.h>

#define HIP_ID 2
#define KNEE_ID 4
#define ANKLE_ID 5

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
    // désactivation des moteurs non utilisés
    dxl_ping(1);
    MX28_TORQUE_SET(1, MX28_ON);
    MX28_LED_SET(1, MX28_OFF);
    dxl_ping(3);
    MX28_TORQUE_SET(3, MX28_OFF);
    MX28_LED_SET(3, MX28_OFF);
    dxl_ping(6);
    MX28_TORQUE_SET(6, MX28_OFF);
    MX28_LED_SET(6, MX28_OFF);
    // connexion aux moteurs utilisés
    dxl_ping(HIP_ID);
    dxl_ping(KNEE_ID);
    dxl_ping(ANKLE_ID);

    log_info("Allumage des LED des moteurs");
    MX28_LED_SET(HIP_ID, MX28_ON);
    MX28_LED_SET(KNEE_ID, MX28_ON);
    MX28_LED_SET(ANKLE_ID, MX28_ON);

    /*
    info_moteur(HIP_ID);
    info_moteur(KNEE_ID);
    info_moteur(ANKLE_ID);
    */

    log_info("Mise en route des moteurs");
    MX28_TORQUE_SET(HIP_ID, MX28_ON);
    MX28_TORQUE_SET(KNEE_ID, MX28_ON);
    MX28_TORQUE_SET(ANKLE_ID, MX28_ON);

    log_info("Mise en position initiale");
    MX28_GOAL_POSITION_SET(HIP_ID, conversion_degre_position(180));
    MX28_GOAL_POSITION_SET(KNEE_ID, conversion_degre_position(220));
    MX28_GOAL_POSITION_SET(ANKLE_ID, conversion_degre_position(200));
    sleep(1);
    MX28_MOVING_SPEED_SET(HIP_ID, 20);
    MX28_MOVING_SPEED_SET(KNEE_ID, 20);
    MX28_MOVING_SPEED_SET(ANKLE_ID, 20);

    log_info("Déplacement du pied et de la hanche");
    MX28_GOAL_POSITION_SET(HIP_ID, conversion_degre_position(200));
    MX28_GOAL_POSITION_SET(ANKLE_ID, conversion_degre_position(180));

    for (uint8_t i = 0; i < 4; i++)
    {
        log_info("Déplacement du genou");
        MX28_GOAL_POSITION_SET(KNEE_ID, conversion_degre_position(110));
        for (uint8_t j = 0; j < 20; j++)
        {
            sleep_ms(100);
            snprintf(
                msg,
                1023,
                "Position genou : %f°",
                conversion_position_degre(MX28_PRESENT_POSITION_GET(KNEE_ID)));
            log_info(msg);
        }

        log_info("Déplacement du genou");
        MX28_GOAL_POSITION_SET(KNEE_ID, conversion_degre_position(180));
        for (uint8_t j = 0; j < 20; j++)
        {
            sleep_ms(100);
            snprintf(
                msg,
                1023,
                "Position genou : %f°",
                conversion_position_degre(MX28_PRESENT_POSITION_GET(KNEE_ID)));
            log_info(msg);
        }
    }

    log_info("Extinction des moteurs");
    sleep(3);
    MX28_TORQUE_SET(HIP_ID, MX28_OFF);
    MX28_TORQUE_SET(KNEE_ID, MX28_OFF);
    MX28_TORQUE_SET(ANKLE_ID, MX28_OFF);
    MX28_TORQUE_SET(1, MX28_OFF);

    log_info("Extinction des LED des moteurs");
    MX28_LED_SET(HIP_ID, MX28_ON);
    MX28_LED_SET(KNEE_ID, MX28_ON);
    MX28_LED_SET(ANKLE_ID, MX28_ON);

    MX28_MOVING_SPEED_SET(HIP_ID, 0);
    MX28_MOVING_SPEED_SET(KNEE_ID, 0);
    MX28_MOVING_SPEED_SET(ANKLE_ID, 0);

    dxl_close();

    return 0;
}
