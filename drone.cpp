#include <Arduino.h>
#include <Servo.h>
#include "standard.h"
#include "drone.h"
#include "mapping.h"
#include "esc.h"
#include "led.h"
#include "mapf.h"
#include "mode.h"
#include "nrf24l01.h"
#include "RC-receiver.h"

BOOLEAN F_no_reception;
BOOLEAN F_start_gyro_calibration;
BOOLEAN F_start_gyro_calibration_mem;
BOOLEAN F_start_esc_calibration;
BOOLEAN F_start_esc_calibration_mem;
REAL32 throttle;
REAL32 roll;
REAL32 pitch;
REAL32 yaw;

static UNS8 ch5_servo_value;
static UNS32 receive_time;
static Servo ch5_servo;

#define MIN_COMMAND 0
#define MAX_COMMAND 255
#define MIN_ESC_COMMAND (REAL32)1000.0f
#define MIN_ESC_COMMAND (REAL32)2000.0f

void initializeDrone(void)
{
    F_no_reception = false;
    F_start_gyro_calibration = false;
    F_start_gyro_calibration_mem = false;
    F_start_esc_calibration = false;
    F_start_esc_calibration_mem = false;
    throttle = MIN_ESC_COMMAND;
    roll = MIN_ESC_COMMAND;
    pitch = MIN_ESC_COMMAND;
    yaw = MIN_ESC_COMMAND;
    receive_time = 0;
    ch5_servo.attach(CH5);
}

void manageDrone(void)
{
    if (readRadio())
    {
        F_no_reception = false;
        receive_time = pit_number;
        ch5_servo_value = map(radio_data.l_potentiometer, 0, 255, 0, 180);  // TBR
        ch5_servo.write(ch5_servo_value);                                   // TBR
        F_start_gyro_calibration_mem = F_start_gyro_calibration;
        F_start_gyro_calibration = radio_data.lo_push_button;
        F_start_esc_calibration_mem = F_start_esc_calibration;
        F_start_esc_calibration = radio_data.r_toggle_switch;
        throttle = mapf((REAL32)radio_data.l_y_joystick, MIN_COMMAND, MAX_COMMAND, MIN_ESC_COMMAND, MIN_ESC_COMMAND);
        yaw = mapf((REAL32)radio_data.l_x_joystick, MIN_COMMAND, MAX_COMMAND, MIN_ESC_COMMAND, MIN_ESC_COMMAND);
        pitch = mapf((REAL32)radio_data.r_y_joystick, MIN_COMMAND, MAX_COMMAND, MIN_ESC_COMMAND, MIN_ESC_COMMAND);
        roll = mapf((REAL32)radio_data.r_x_joystick, MIN_COMMAND, MAX_COMMAND, MIN_ESC_COMMAND, MIN_ESC_COMMAND);
    }
    else
    {
        if ((pit_number - receive_time) > (5000/PIT_PERIOD))
        {
            F_no_reception = true;
        }
        else
        {
        }
    }
    manageMode();
}