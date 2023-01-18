#include <Arduino.h>
#include "standard.h"
#include "mode.h"
#include "drone.h"
#include "led.h"
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
    FLIGHT
} T_mode;

static T_mode mode;
#ifdef LCD
static rgb_lcd lcd;
#endif // LCD
bfs::SbusTx sbus_tx(&Serial);
bfs::SbusData sbus_data;

void initializeMode(void)
{
    UNS8 i;

#ifdef SPY
    Serial.println("Mode : NONE");
#endif // SPY
    mode = NONE;
#ifdef LCD
    lcd.begin(16, 2);
    lcd.clear();
    lcd.print("NONE");
#endif // LCD
    sbus_tx.Begin();
    for(i=0;i < sbus_data.NUM_CH;i++) {
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
                sbus_data.ch[0] = 0;
                sbus_data.ch[1] = 0;
                sbus_data.ch[2] = 0;
                sbus_data.ch[3] = 0;
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
                sbus_data.ch[0] = 0;
                sbus_data.ch[1] = 0;
                sbus_data.ch[2] = 0;
                sbus_data.ch[3] = 0;
                sbus_tx.data(sbus_data);
                mode = NONE;
            }
            else
            {
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
                sbus_data.ch[0] = 0;
                sbus_data.ch[1] = 0;
                sbus_data.ch[2] = 0;
                sbus_data.ch[3] = 0;
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
#ifdef LCD
                lcd.clear();
                lcd.print("STOP");
#endif // LCD
                sbus_data.ch[0] = 0;
                sbus_data.ch[1] = 0;
                sbus_data.ch[2] = 0;
                sbus_data.ch[3] = 0;
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
                sbus_data.ch[0] = 0;
                sbus_data.ch[1] = 0;
                sbus_data.ch[2] = 0;
                sbus_data.ch[3] = 0;
                sbus_tx.data(sbus_data);
                mode = NONE;
            }
            else
            {
                sbus_data.ch[0] = (INT16)roll;
                sbus_data.ch[1] = (INT16)pitch;
                sbus_data.ch[2] = (INT16)yaw;
                sbus_data.ch[3] = (INT16)throttle;
                sbus_tx.data(sbus_data);
                sbus_tx.Write();
            }
            break;
    }
}