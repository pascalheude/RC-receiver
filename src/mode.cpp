#include <Arduino.h>
#include "standard.h"
#include "mode.h"
#include "drone.h"
#include "led.h"
#include "mathf.h"
#include "nrf24l01.h"
#include "RC-receiver.h"
#ifdef LCD
#include "rgb_lcd.h"
#endif // LCD
#include "sbus.h"
#include "spy.h"
 
typedef enum {
    NONE = 0,
    STOP,
    STARTING,
    FLIGHT,
    TEST
} T_mode;

#define ROLL 0
#define PITCH 1
#define YAW 2
#define THROTTLE 3

static UNS32 r_toggle_switch_time;
static UNS32 lo_push_button_time;
static UNS32 ro_push_button_time;
static REAL32 test_throttle;
static T_mode mode;
#ifdef LCD
static rgb_lcd lcd;
#endif // LCD
bfs::SbusTx sbus_tx(&Serial); // Do not use inversion, not working on Arduino
bfs::SbusData sbus_data;

void initializeMode(void)
{
    UNS8 i;

#ifdef SPY
    Serial.println("Mode : NONE");
#endif // SPY
    r_toggle_switch_time = 0;
    lo_push_button_time = 0;
    ro_push_button_time = 0;
    test_throttle = MIN_ESC_COMMAND;
    mode = NONE;
#ifdef LCD
    lcd.begin(16, 2);
    lcd.clear();
    lcd.print("NONE");
#endif // LCD
    sbus_tx.Begin();
    sbus_data.ch[ROLL] = MIN_ESC_COMMAND;
    sbus_data.ch[PITCH] = MIN_ESC_COMMAND;
    sbus_data.ch[YAW] = MIN_ESC_COMMAND;
    sbus_data.ch[THROTTLE] = MIN_ESC_COMMAND;
    for(i=(THROTTLE+1);i < sbus_data.NUM_CH;i++)
    {
        sbus_data.ch[i] = 0;
    }
    sbus_data.ch17 = false;
    sbus_data.ch18 = false;
    sbus_data.failsafe = false;
    sbus_data.lost_frame = false;
}

void manageMode(void)
{
    switch(mode)
    {
        default :
        case NONE :
            blinkLed2(500);
            blinkLed3(500);
            if (F_one_reception_ok)
            {
#ifdef SPY
                Serial.println("Mode : NONE -> STOP");
#endif // SPY
#ifdef LCD
                lcd.clear();
                lcd.print("STOP");
#endif // LCD
                sbus_data.ch[ROLL] = MIN_ESC_COMMAND;
                sbus_data.ch[PITCH] = MIN_ESC_COMMAND;
                sbus_data.ch[YAW] = MIN_ESC_COMMAND;
                sbus_data.ch[THROTTLE] = MIN_ESC_COMMAND;
                sbus_tx.data(sbus_data);
                mode = STOP;
            }
            else
            {
            }
            break;
        case STOP :
            switchOffLed2();
            switchOnLed3();
            sbus_tx.Write();
            if ((yaw <= (REAL32)1012.0f) && (throttle <= (REAL32)1012.0f))
            {
#ifdef LCD
                lcd.clear();
                lcd.print("STARTING");
#endif // LCD
                r_toggle_switch_time = 0;
                mode = STARTING;
            }
            else if (F_no_reception)
            {
#ifdef SPY
                Serial.println("Mode : STOP -> NONE");
#endif // SPY
#ifdef LCD
                lcd.clear();
                lcd.print("NONE");
#endif // LCD
                r_toggle_switch_time = 0;
                sbus_data.ch[ROLL] = MIN_ESC_COMMAND;
                sbus_data.ch[PITCH] = MIN_ESC_COMMAND;
                sbus_data.ch[YAW] = MIN_ESC_COMMAND;
                sbus_data.ch[THROTTLE] = MIN_ESC_COMMAND;
                sbus_tx.data(sbus_data);
                mode = NONE;
            }
            else if (r_toggle_switch == true)
            {
                if (r_toggle_switch_time == 0)
                {
                    r_toggle_switch_time = pit_number;
                }
                else if ((pit_number - r_toggle_switch_time) > (5000/PIT_PERIOD))
                {
#ifdef SPY
                    Serial.println("Mode : STOP -> TEST");
#endif // SPY
#ifdef LCD
                    lcd.clear();
                    lcd.print("TEST");
#endif // LCD
                    r_toggle_switch_time = 0;
                    sbus_data.ch[ROLL] = MIN_ESC_COMMAND;
                    sbus_data.ch[PITCH] = MIN_ESC_COMMAND;
                    sbus_data.ch[YAW] = MIN_ESC_COMMAND;
                    sbus_data.ch[THROTTLE] = MIN_ESC_COMMAND;
                    sbus_tx.data(sbus_data);
                    mode = TEST;
                }
                else
                {
                }
            }
            else
            {
                r_toggle_switch_time = 0;
            }
            break;
        case STARTING :
            switchOnLed2();
            switchOffLed3();
            sbus_tx.Write();
            if ((yaw >= (REAL32)1488.0f) && (yaw <= (REAL32)1512.0f) &&
                (throttle <= (REAL32)1012.0f))
            {
#ifdef LCD
                lcd.clear();
                lcd.print("FLIGHT");
#endif // LCD
                mode = FLIGHT;
            }
            else if (F_no_reception)
            {
#ifdef SPY
                Serial.println("Mode : STARTING -> NONE");
#endif // SPY
#ifdef LCD
                lcd.clear();
                lcd.print("NONE");
#endif // LCD
                sbus_data.ch[ROLL] = MIN_ESC_COMMAND;
                sbus_data.ch[PITCH] = MIN_ESC_COMMAND;
                sbus_data.ch[YAW] = MIN_ESC_COMMAND;
                sbus_data.ch[THROTTLE] = MIN_ESC_COMMAND;
                sbus_tx.data(sbus_data);
                mode = NONE;
            }
            else
            {
            }
            break;
        case FLIGHT :
            switchOnLed2();
            switchOnLed3();
            if ((yaw >= (REAL32)1988.0f) && (throttle <= (REAL32)1012.0f))
            {
#ifdef SPY
                Serial.println("Mode : FLIGHT -> STOP");
#endif // SPY
#ifdef LCD
                lcd.clear();
                lcd.print("STOP");
#endif // LCD
                sbus_data.ch[ROLL] = MIN_ESC_COMMAND;
                sbus_data.ch[PITCH] = MIN_ESC_COMMAND;
                sbus_data.ch[YAW] = MIN_ESC_COMMAND;
                sbus_data.ch[THROTTLE] = MIN_ESC_COMMAND;
                sbus_tx.data(sbus_data);
                mode = STOP;
            }
            else if (F_no_reception)
            {
#ifdef SPY
                Serial.println("Mode : FLIGHT -> NONE");
#endif // SPY
#ifdef LCD
                lcd.clear();
                lcd.print("NONE");
#endif // LCD
                sbus_data.ch[ROLL] = MIN_ESC_COMMAND;
                sbus_data.ch[PITCH] = MIN_ESC_COMMAND;
                sbus_data.ch[YAW] = MIN_ESC_COMMAND;
                sbus_data.ch[THROTTLE] = MIN_ESC_COMMAND;
                sbus_tx.data(sbus_data);
                mode = NONE;
            }
            else
            {
                sbus_data.ch[ROLL] = (INT16)roll;
                sbus_data.ch[PITCH] = (INT16)pitch;
                sbus_data.ch[YAW] = (INT16)yaw;
                sbus_data.ch[THROTTLE] = (INT16)throttle;
                sbus_tx.data(sbus_data);
                sbus_tx.Write();
            }
            break;
        case TEST :
            blinkLed2And3(500);
            if (r_toggle_switch == false)
            {
#ifdef SPY
                Serial.println("Mode : TEST -> STOP");
#endif // SPY
#ifdef LCD
                lcd.clear();
                lcd.print("STOP");
#endif // LCD
                sbus_data.ch[THROTTLE] = MIN_ESC_COMMAND;
                sbus_tx.data(sbus_data);
                lo_push_button_time = 0;
                ro_push_button_time = 0;
                mode = STOP;
            }
            else if (F_no_reception)
            {
#ifdef SPY
                Serial.println("Mode : FLIGHT -> NONE");
#endif // SPY
#ifdef LCD
                lcd.clear();
                lcd.print("NONE");
#endif // LCD
                sbus_data.ch[THROTTLE] = MIN_ESC_COMMAND;
                sbus_tx.data(sbus_data);
                lo_push_button_time = 0;
                ro_push_button_time = 0;
                mode = NONE;
            }
            else if (li_push_button == true)
            {
#ifdef LCD
                lcd.setCursor(1,1);
                lcd.print((UNS16)test_throttle);
#endif // LCD
                lo_push_button_time = 0;
                ro_push_button_time = 0;
                test_throttle = MIN_ESC_COMMAND;
                sbus_data.ch[THROTTLE] = MIN_ESC_COMMAND;
                sbus_tx.data(sbus_data);
                sbus_tx.Write();
            }
            else if (lo_push_button == true)
            {
                ro_push_button_time = 0;
                if (lo_push_button_time == 0)
                {
                    lo_push_button_time = pit_number;
                }
                else if ((pit_number - lo_push_button_time) > (500/PIT_PERIOD))
                {
                    lo_push_button_time = 0;
                    test_throttle = LIMIT(test_throttle - 50, MIN_ESC_COMMAND, MAX_ESC_COMMAND);
#ifdef LCD
                    lcd.setCursor(1,1);
                    lcd.print((UNS16)test_throttle);
#endif // LCD
                }
                else
                {
                }
                sbus_data.ch[THROTTLE] = (INT16)test_throttle;
                sbus_tx.data(sbus_data);
                sbus_tx.Write();
            }
            else if (ro_push_button == true)
            {
                lo_push_button_time = 0;
                if (ro_push_button_time == 0)
                {
                    ro_push_button_time = pit_number;
                }
                else if ((pit_number - ro_push_button_time) > (500/PIT_PERIOD))
                {
                    ro_push_button_time = 0;
                    test_throttle = LIMIT(test_throttle + 50, MIN_ESC_COMMAND, MAX_ESC_COMMAND);
#ifdef LCD
                    lcd.setCursor(1,1);
                    lcd.print((UNS16)test_throttle);
#endif // LCD
                }
                else
                {
                }
                sbus_data.ch[THROTTLE] = (INT16)test_throttle;
                sbus_tx.data(sbus_data);
                sbus_tx.Write();
            }
            else
            {
                lo_push_button_time = 0;
                ro_push_button_time = 0;
                sbus_tx.data(sbus_data);
                sbus_tx.Write();
           }
            break;
    }
}