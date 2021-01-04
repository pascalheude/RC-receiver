#include <Arduino.h>
#include <Servo.h>
#include "standard.h"
#include "esc.h"
#include "mapping.h"
#include "nrf24l01.h"
#include "RC-receiver.h"

UNS16 lf_esc_value;
UNS16 rf_esc_value;
UNS16 lr_esc_value;
UNS16 rr_esc_value;

#define ESC_TMO 500

static Servo lf_esc;
static Servo rf_esc;
static Servo lr_esc;
static Servo rr_esc;
static Servo *esc;
static UNS8 esc_state;
static UNS32 esc_time;

void initializeEsc(void)
{
    lf_esc.attach(CH1);
    rf_esc.attach(CH2);
    lr_esc.attach(CH3);
    rr_esc.attach(CH4);
    esc = 0;
    esc_state = 0;
    esc_time = 0;
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
            esc->write(90);
            esc_time = 0;
            esc_state = 1;
            break;
        case 1 :
            if (radio_data.ro_push_button == true)
            {
                esc->write(180);
                esc_time = pit_number;
                esc_state = 2;
            }
            else
            {
            }
            break;
        case 2 :
            if ((pit_number - esc_time) >= (ESC_TMO/PIT_PERIOD)) 
            {
                esc_state = 3;
            }
            else
            {
            }
            break;
        case 3 :
            if (radio_data.ro_push_button == true)
            {
                esc->write(0);
                esc_time = pit_number;
                esc_state = 4;
            }
            else
            {
            }
            break;
        case 4 :
            if ((pit_number - esc_time) >= (ESC_TMO/PIT_PERIOD)) 
            {
                esc_state = 5;
            }
            else
            {
            }
            break;
        case 5 :
            esc->write(0);
            esc_time = 0;
            esc_state = 0;
            return(true);
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