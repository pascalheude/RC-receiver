#include <Arduino.h>
#include <Servo.h>
#include "standard.h"
#include "mapping.h"
#include "mode.h"
#include "nrf24l01.h"

BOOLEAN F_start_calibration;

Servo lf_esc;
Servo rf_esc;
Servo lr_esc;
Servo rr_esc;
Servo ch5_servo;
UNS8 ch5_servo_value;
UNS16 lf_esc_value;
UNS16 rf_esc_value;
UNS16 lr_esc_value;
UNS16 rr_esc_value;

void initializeDrone(void)
{
    F_start_calibration = false;
    lf_esc.attach(CH1);
    rf_esc.attach(CH2);
    lr_esc.attach(CH3);
    rr_esc.attach(CH4);
    ch5_servo.attach(CH5);
}

void manageDrone(void)
{
    if (readRadio())
    {
        ch5_servo_value = map(radio_data.l_potentiometer, 0, 255, 0, 180);
        ch5_servo.write(ch5_servo_value);
        lf_esc_value = map(radio_data.l_potentiometer, 0, 255, 1000, 2000);
        lf_esc.writeMicroseconds(lf_esc_value);
        F_start_calibration = radio_data.lo_push_button;
    }
    else
    {
    }
    manageMode();
}