#include <Arduino.h>
#include <Servo.h>
#include "standard.h"
#include "drone.h"
#include "mapping.h"
#include "led.h"
#include "mathf.h"
#include "mode.h"
#include "not.h"
#include "nrf24l01.h"
#include "RC-receiver.h"

BOOLEAN F_one_reception_ok;
BOOLEAN F_no_reception;
BOOLEAN r_toggle_switch;
BOOLEAN li_push_button;
BOOLEAN lo_push_button;
BOOLEAN ro_push_button;
INT16 l_potentiometer;
INT16 r_potentiometer;
REAL32 throttle;
REAL32 roll;
REAL32 pitch;
REAL32 yaw;

static UNS8 ch5_servo_value; // TBR
static UNS32 receive_time;
static Servo ch5_servo; // TBR

#define MAX_ESC_COMMAND 1792
#define MINF_ESC_COMMAND (REAL32)192.0f
#define NOF_ESC_COMMAND (REAL32)992.0f
#define MAXF_ESC_COMMAND (REAL32)1792.0f
#define MIN_COMMAND 0
#define MAX_COMMAND 255
#define MINF_COMMAND (REAL32)0.0f
#define MAXF_COMMAND (REAL32)255.0f

void initializeDrone(void)
{
    F_one_reception_ok = false;
    F_no_reception = false;
    r_toggle_switch = false;
    li_push_button = false;
    lo_push_button = false;
    ro_push_button = false;
    l_potentiometer = MIN_ESC_COMMAND;
    r_potentiometer = MIN_ESC_COMMAND;
    throttle = MINF_ESC_COMMAND;
    roll = MINF_ESC_COMMAND;
    pitch = MINF_ESC_COMMAND;
    yaw = MINF_ESC_COMMAND;
    receive_time = 0;
    ch5_servo.attach(CH5); // TBR
}

void manageDrone(void)
{
    if (readRadio())
    {
        F_one_reception_ok = true;
        F_no_reception = false;
        receive_time = pit_number;
        ch5_servo_value = map(radio_data.l_potentiometer, 0, 255, 0, 180);  // TBR
        ch5_servo.write(ch5_servo_value);                                   // TBR
        r_toggle_switch = NOT(radio_data.r_toggle_switch);
        li_push_button = NOT(radio_data.li_push_button);
        lo_push_button = NOT(radio_data.lo_push_button);
        ro_push_button = NOT(radio_data.ro_push_button);
        if ((126 <= radio_data.l_y_joystick) && (radio_data.l_y_joystick <= 128))
        {
            throttle = NOF_ESC_COMMAND;
        }
        else
        {
            throttle = mapf((REAL32)radio_data.l_y_joystick, MINF_COMMAND, MAXF_COMMAND, MINF_ESC_COMMAND, MAXF_ESC_COMMAND);
        }
        if ((126 <= radio_data.l_x_joystick) && (radio_data.l_x_joystick <= 128))
        {
            yaw = NOF_ESC_COMMAND;
        }
        else
        {
            yaw = mapf((REAL32)radio_data.l_x_joystick, MAXF_COMMAND, MINF_COMMAND, MINF_ESC_COMMAND, MAXF_ESC_COMMAND);
        }
        if ((126 <= radio_data.r_y_joystick) && (radio_data.r_y_joystick <= 128))
        {
            pitch = NOF_ESC_COMMAND;
        }
        else
        {
            pitch = mapf((REAL32)radio_data.r_y_joystick, MINF_COMMAND, MAXF_COMMAND, MINF_ESC_COMMAND, MAXF_ESC_COMMAND);
        }
        if ((126 <= radio_data.r_x_joystick) && (radio_data.r_x_joystick <= 128))
        {
            roll = NOF_ESC_COMMAND;
        }
        else
        {
            roll = mapf((REAL32)radio_data.r_x_joystick, MAXF_COMMAND, MINF_COMMAND, MINF_ESC_COMMAND, MAXF_ESC_COMMAND);
        }
        l_potentiometer = map(radio_data.l_potentiometer, MIN_COMMAND, MAX_COMMAND, MIN_ESC_COMMAND, MAX_ESC_COMMAND);
        r_potentiometer = map(radio_data.r_potentiometer, MIN_COMMAND, MAX_COMMAND, MIN_ESC_COMMAND, MAX_ESC_COMMAND);
    }
    else
    {
        F_one_reception_ok = false;
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