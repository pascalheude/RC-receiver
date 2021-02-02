#include <Arduino.h>
#include "standard.h"
#include "mode.h"
#include "drone.h"
#include "esc.h"
#include "led.h"
#include "mpu6050.h"
#include "nrf24l01.h"
#include "RC-receiver.h"

typedef enum {
#ifdef SPY
    NONE = 0,
    STARTUP,
#else
    STARTUP = 0,
#endif // SPY
    CALIBRATION_GYRO,
    START_ESC_CALIBRATION,
    CALIBRATION_ESC_LF,
    CALIBRATION_ESC_RF,
    CALIBRATION_ESC_LR,
    CALIBRATION_ESC_RR,
    FLIGHT
} T_mode;

static BOOLEAN F_esc_calibration_done;
static T_mode mode;

void initializeMode(void)
{
    F_esc_calibration_done = false;
#ifdef SPY
    mode = NONE;
#else
    mode = STARTUP;
#endif // SPY
}

void manageMode(void)
{
    //driveEsc(LF, lf_esc_value); // TBR
    switch(mode)
    {
#ifdef SPY
        case NONE :
            mode = STARTUP;
            Serial.println("Mode : NONE -> STARTUP");
            break;
#endif // SPY
        case STARTUP :
            if ((F_start_gyro_calibration_mem == true) && 
                (F_start_gyro_calibration == false))
            {
#ifdef SPY
                Serial.println("Mode : STARTUP -> CALIBRATION_GYRO");
#endif // SPY
                mode = CALIBRATION_GYRO;
                switchOffLed2();
            }
            else
            {
                blinkLed2(500);
            }
            break;
        case CALIBRATION_GYRO :
            if (calibrateMpu6050())
            {
#ifdef SPY
                Serial.println("Mode : CALIBRATION_GYRO -> FLIGHT");
#endif // SPY
                mode = FLIGHT;
                switchOffLed3();
            }
            else
            {
                blinkLed3(500);
            }
            break;
        case START_ESC_CALIBRATION :
            startEscCalibration();
            if (radio_data.li_push_button == true)
            {
                mode = CALIBRATION_ESC_LF;
            }
            else
            {
            }
            break;
        case CALIBRATION_ESC_LF :
            if (calibrateEsc(LF))
            {
                mode = CALIBRATION_ESC_RF;
                switchOnLed2();
            }
            else
            {
            }
            break;
        case CALIBRATION_ESC_RF :
            if (calibrateEsc(RF))
            {
                mode = CALIBRATION_ESC_RR;
                switchOffLed2();
                switchOnLed3();
            }
            else
            {
            }
            break;
        case CALIBRATION_ESC_RR :
            if (calibrateEsc(RR))
            {
                mode = CALIBRATION_ESC_LR;
                switchOnLed2();
            }
            else
            {
            }
            break;
        case CALIBRATION_ESC_LR :
            if (calibrateEsc(LR))
            {
                F_esc_calibration_done = true;
                mode = FLIGHT;
            }
            else
            {
            }
            break;
        case FLIGHT :
            if ((F_esc_calibration_done == false) &&
                (F_start_esc_calibration_mem == true) &&
                (F_start_esc_calibration == false))
            {
#ifdef SPY
                Serial.println("Mode : FLIGHT -> CALIBRATION_ESC_LF");
#endif // SPY
                mode = START_ESC_CALIBRATION;
                switchOffLed2();
                switchOffLed3();
            }
            else
            {
                blinkLed2(500);
                blinkLed3(500);
                readSensor();
                calculateAngle();
            }
            break;
    }
}