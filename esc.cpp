#include <Arduino.h>
#include <Servo.h>
#include "standard.h"
#include "esc.h"
#include "mapping.h"
#include "RC-receiver.h"

UNS16 lf_esc_value;
UNS16 rf_esc_value;
UNS16 lr_esc_value;
UNS16 rr_esc_value;

#define ESC_TMO 1000

static UNS8 esc_state;
static UNS32 esc_time;
static Servo lf_esc;
static Servo rf_esc;
static Servo lr_esc;
static Servo rr_esc;
static Servo *esc;

void initializeEsc(void)
{
    esc_state = 0;
    esc_time = 0;
    lf_esc.attach(CH1);
    lf_esc.write(0);
    rf_esc.attach(CH2);
    rf_esc.write(0);
    lr_esc.attach(CH3);
    lr_esc.write(0);
    rr_esc.attach(CH4);
    rr_esc.write(0);
    esc = 0;
}

void startEscCalibration(void)
{
    lf_esc.write(90);
    rf_esc.write(90);
    lr_esc.write(90);
    rr_esc.write(90);
}

BOOLEAN calibrateEsc(T_esc_position esc_position)
{
    switch (esc_state)
    {
        case 0 :
            switch (esc_position)
            {
                case LF :
                    esc = &lf_esc;
                    break;
                case RF :
                    esc = &rf_esc;
                    break;
                case LR :
                    esc = &lr_esc;
                    break;
                case RR :
                    esc = &rr_esc;
                    break;
            }
            esc->write(180);
            esc_time = pit_number;
            esc_state = 1;
            break;
        case 1 :
            if ((pit_number - esc_time) >= (ESC_TMO/PIT_PERIOD)) 
            {
                esc_state = 2;
            }
            else
            {
            }
            break;
        case 2 :
            esc->write(0);
            esc_time = pit_number;
            esc_state = 3;
            break;
        case 3 :
            if ((pit_number - esc_time) >= (ESC_TMO/PIT_PERIOD)) 
            {
                esc_time = 0;
                esc_state = 0;
                return(true);
            }
            else
            {
            }
            break;
        default :
            esc_state = 0;
            break;
    }
    return(false);
}

void driveEsc(T_esc_position esc_position, UNS16 value)
{
    switch (esc_position)
    {
        case LF :
            lf_esc.writeMicroseconds(value);
            break;
        case RF :
            rf_esc.writeMicroseconds(value);
            break;
        case LR :
            lr_esc.writeMicroseconds(value);
            break;
        case RR :
            rr_esc.writeMicroseconds(value);
            break;
    }
}