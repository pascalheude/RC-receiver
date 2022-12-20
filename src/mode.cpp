#include <Arduino.h>
#include "standard.h"
#include "mode.h"
#include "drone.h"
#include "esc.h"
#include "led.h"
#include "mpu6050.h"
#include "pid.h"
#include "nrf24l01.h"
#include "RC-receiver.h"
#ifdef LCD
#include "rgb_lcd.h"
#endif // LCD
#include "spy.h"
 
typedef enum {
    NONE = 0,
    STARTUP,
    CALIBRATION_GYRO,
    STOP,
    STARTING,
    FLIGHT
} T_mode;

static T_mode mode;
#ifdef LCD
static rgb_lcd lcd;
#endif // LCD

void initializeMode(void)
{
#ifdef SPY
    Serial.println("Mode : NONE");
#endif // SPY
    mode = NONE;
#ifdef LCD
    lcd.begin(16, 2);
#endif // LCD
}

void manageMode(void)
{
    switch(mode)
    {
        case NONE :
            if (F_one_reception_ok)
            {
#ifdef SPY
                Serial.println("Mode : NONE -> STARTUP");
#endif // SPY
                lcd.print("STARTUP");
                lcd.setCursor(0, 1);
                lcd.print("LO->GYRO CALIB.");
                mode = STARTUP;
            }
            else
            {
            }
            break;
        case STARTUP :
            if ((F_start_gyro_calibration_mem == true) && 
                (F_start_gyro_calibration == false))
            {
#ifdef SPY
                Serial.println("Mode : STARTUP -> CALIBRATION_GYRO");
#endif // SPY
#ifdef LCD
                lcd.clear();
                lcd.print("CALIBRATION GYRO");
#endif // LCD
                mode = CALIBRATION_GYRO;
                switchOnLed2();
                switchOffLed3();
            }
            else
            {
                blinkLed2And3(500);
            }
            break;
        case CALIBRATION_GYRO :
            if (calibrateMpu6050())
            {
#ifdef SPY
                Serial.println("Mode : CALIBRATION_GYRO -> STOP");
#endif // SPY
#ifdef LCD
                lcd.clear();
                lcd.print("STOP");
#endif // LCD
                mode = STOP;
                switchOffLed2();
            }
            else
            {
            }
            break;
        case STOP :
            blinkLed2(1000);
            blinkLed3(1000);
            if ((yaw <= (REAL32)1012.0f) && (throttle <= (REAL32)1012.0f))
            {
#ifdef LCD
                lcd.clear();
                lcd.print("STARTING");
#endif // LCD
                mode = STARTING;
            }
            else
            {
            }
            break;
        case STARTING :
            if ((yaw >= (REAL32)1488.0f) && (yaw <= (REAL32)1512.0f) &&
                (throttle <= (REAL32)1012.0f))
            {
#ifdef LCD
                lcd.clear();
                lcd.print("FLIGHT");
#endif // LCD
                mode = FLIGHT;
                resetGyroAngle();
                resetPid();
            }
            else
            {
                blinkLed2(500);
                blinkLed3(500);
            }
            break;
        case FLIGHT :
            if ((yaw >= (REAL32)1988.0f) && (throttle <= (REAL32)1012.0f))
            {
#ifdef LCD
                lcd.clear();
                lcd.print("STOP");
#endif // LCD
                mode = STOP;
                stopEsc();
            }
            else
            {
                blinkLed2(250);
                blinkLed3(250);
                readSensor();
                calculateAngle();
                performPid();
                driveEsc();
            }
            break;
    }
}