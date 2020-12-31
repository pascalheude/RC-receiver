#include <Arduino.h>
#include "standard.h"
#include "drone.h"
#include "mode.h"
#include "mpu6050.h"

typedef enum {
    STARTUP = 0,
    CALIBRATION_GYRO,
    CALIBRATION_ECS,
    FLIGHT
} T_mode;

static T_mode mode;

void initializeMode(void)
{
    mode = STARTUP;
}

void manageMode(void)
{
    switch(mode)
    {
        case STARTUP :
            if (F_start_calibration)
            {
                mode = CALIBRATION_GYRO;
            }
            else
            {
                
            }
            break;
        case CALIBRATION_GYRO :
            if (calibrateMpu6050())
            {
                mode = CALIBRATION_ECS;
            }
            else
            {
            }
            break;
        case CALIBRATION_ECS :
            break;
        case FLIGHT :
            break;
    }
    F_start_calibration = false;
}