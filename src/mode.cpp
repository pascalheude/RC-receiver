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

#define THROTTLE 0
#define ROLL 1
#define PITCH 2
#define YAW 3
#define ARM 4
#define MIN_THROTTLE 264  // 1045
#define MAX_THROTTLE 1728 // 1960
#define NO_ARM_COMMAND 192 // 1000
#define ARM_COMMAND 1152 // 1600


static INT16 test_throttle;
static UNS32 r_toggle_switch_time;
static UNS32 lo_push_button_time;
static UNS32 ro_push_button_time;
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
    test_throttle = MIN_THROTTLE;
    mode = NONE;
#ifdef LCD
    lcd.begin(16, 2);
    lcd.clear();
    lcd.print("NONE");
#endif // LCD
    sbus_tx.Begin();
    sbus_data.ch[ROLL] = NO_ESC_COMMAND;
    sbus_data.ch[PITCH] = NO_ESC_COMMAND;
    sbus_data.ch[YAW] = NO_ESC_COMMAND;
    sbus_data.ch[THROTTLE] = MIN_ESC_COMMAND;
    sbus_data.ch[ARM] = NO_ARM_COMMAND;
    for(i=(ARM+1);i < sbus_data.NUM_CH;i++)
    {
        sbus_data.ch[i] = MIN_ESC_COMMAND;
    }
    sbus_data.ch17 = false;
    sbus_data.ch18 = false;
    sbus_data.failsafe = false;
    sbus_data.lost_frame = false;
    sbus_tx.data(sbus_data);
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
                mode = STOP;
            }
            else
            {
            }
            sbus_data.ch[5] = l_potentiometer;
            sbus_data.ch[6] = r_potentiometer;
            sbus_data.lost_frame = F_frame_lost;
            sbus_data.failsafe = F_failsafe;
            sbus_tx.data(sbus_data);
            sbus_tx.Write();
            break;
        case STOP :
            switchOffLed2();
            switchOnLed3();
            if ((yaw <= (REAL32)212.0f) && (throttle <= (REAL32)212.0f))
            {
#ifdef LCD
                lcd.clear();
                lcd.print("STARTING");
#endif // LCD
                r_toggle_switch_time = 0;
                mode = STARTING;
            }
            else if (F_failsafe)
            {
#ifdef SPY
                Serial.println("Mode : STOP -> NONE");
#endif // SPY
#ifdef LCD
                lcd.clear();
                lcd.print("NONE");
#endif // LCD
                r_toggle_switch_time = 0;
                sbus_data.ch[ROLL] = NO_ESC_COMMAND;
                sbus_data.ch[PITCH] = NO_ESC_COMMAND;
                sbus_data.ch[YAW] = NO_ESC_COMMAND;
                sbus_data.ch[THROTTLE] = MIN_ESC_COMMAND;
                sbus_data.ch[ARM] = NO_ARM_COMMAND;
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
                    sbus_data.ch[ROLL] = NO_ESC_COMMAND;
                    sbus_data.ch[PITCH] = NO_ESC_COMMAND;
                    sbus_data.ch[YAW] = NO_ESC_COMMAND;
                    sbus_data.ch[THROTTLE] = MIN_ESC_COMMAND;
                    sbus_data.ch[ARM] = ARM_COMMAND;
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
            sbus_data.ch[5] = l_potentiometer;
            sbus_data.ch[6] = r_potentiometer;
            sbus_data.lost_frame = F_frame_lost;
            sbus_data.failsafe = F_failsafe;
            sbus_tx.data(sbus_data);
            sbus_tx.Write();
            break;
        case STARTING :
            switchOnLed2();
            switchOffLed3();
            if ((yaw >= (REAL32)972.0f) && (yaw <= (REAL32)1011.0f) &&
                (throttle <= (REAL32)212.0f))
            {
#ifdef LCD
                lcd.clear();
                lcd.print("FLIGHT");
#endif // LCD
                sbus_data.ch[ARM] = ARM_COMMAND;
                mode = FLIGHT;
            }
            else if (F_failsafe)
            {
#ifdef SPY
                Serial.println("Mode : STARTING -> NONE");
#endif // SPY
#ifdef LCD
                lcd.clear();
                lcd.print("NONE");
#endif // LCD
                sbus_data.ch[ROLL] = NO_ESC_COMMAND;
                sbus_data.ch[PITCH] = NO_ESC_COMMAND;
                sbus_data.ch[YAW] = NO_ESC_COMMAND;
                sbus_data.ch[THROTTLE] = MIN_ESC_COMMAND;
                sbus_data.ch[ARM] = NO_ARM_COMMAND;
                mode = NONE;
            }
            else
            {
            }
            sbus_data.ch[5] = l_potentiometer;
            sbus_data.ch[6] = r_potentiometer;
            sbus_data.lost_frame = F_frame_lost;
            sbus_data.failsafe = F_failsafe;
            sbus_tx.data(sbus_data);
            sbus_tx.Write();
            break;
        case FLIGHT :
            switchOnLed2();
            switchOnLed3();
            if ((yaw >= (REAL32)1772.0f) && (throttle <= (REAL32)212.0f))
            {
#ifdef SPY
                Serial.println("Mode : FLIGHT -> STOP");
#endif // SPY
#ifdef LCD
                lcd.clear();
                lcd.print("STOP");
#endif // LCD
                sbus_data.ch[ROLL] = NO_ESC_COMMAND;
                sbus_data.ch[PITCH] = NO_ESC_COMMAND;
                sbus_data.ch[YAW] = NO_ESC_COMMAND;
                sbus_data.ch[THROTTLE] = MIN_ESC_COMMAND;
                sbus_data.ch[ARM] = NO_ARM_COMMAND;
                mode = STOP;
            }
            else
            {
                sbus_data.ch[ROLL] = (INT16)roll;
                sbus_data.ch[PITCH] = (INT16)pitch;
                sbus_data.ch[YAW] = (INT16)yaw;
                if (throttle < MIN_THROTTLE)
                {
                    sbus_data.ch[THROTTLE] = MIN_ESC_COMMAND;
                }
                else if (throttle > MAX_THROTTLE)
                {
                    sbus_data.ch[THROTTLE] = MAX_THROTTLE;
                }
                else
                {
                    sbus_data.ch[THROTTLE] = (INT16)throttle;
                }
                sbus_data.ch[THROTTLE] = (INT16)throttle;
            }
            sbus_data.ch[5] = l_potentiometer;
            sbus_data.ch[6] = r_potentiometer;
            sbus_data.lost_frame = F_frame_lost;
            sbus_data.failsafe = F_failsafe;
            sbus_tx.data(sbus_data);
            sbus_tx.Write();
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
                sbus_data.ch[ARM] = NO_ARM_COMMAND;
                lo_push_button_time = 0;
                ro_push_button_time = 0;
                mode = STOP;
            }
            else if (li_push_button == true)
            {
                lo_push_button_time = 0;
                ro_push_button_time = 0;
                test_throttle = MIN_ESC_COMMAND;
                sbus_data.ch[THROTTLE] = MIN_ESC_COMMAND;
#ifdef LCD
                lcd.setCursor(1,1);
                lcd.print(test_throttle);
                lcd.print(" ");
#endif // LCD
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
                    test_throttle = LIMIT(test_throttle - 50, MIN_ESC_COMMAND, MAX_THROTTLE);
#ifdef LCD
                    lcd.setCursor(1,1);
                    lcd.print(test_throttle);
                    lcd.print(" ");
#endif // LCD
                }
                else
                {
                }
                sbus_data.ch[THROTTLE] = (INT16)test_throttle;
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
                    test_throttle = LIMIT(test_throttle + 50, MIN_ESC_COMMAND, MAX_THROTTLE);
#ifdef LCD
                    lcd.setCursor(1,1);
                    lcd.print(test_throttle);
#endif // LCD
                }
                else
                {
                }
                sbus_data.ch[THROTTLE] = (INT16)test_throttle;
            }
            else
            {
                lo_push_button_time = 0;
                ro_push_button_time = 0;
            }
            sbus_data.ch[5] = l_potentiometer;
            sbus_data.ch[6] = r_potentiometer;
            sbus_data.lost_frame = F_frame_lost;
            sbus_data.failsafe = F_failsafe;
            sbus_tx.data(sbus_data);
            sbus_tx.Write();
            break;
    }
}