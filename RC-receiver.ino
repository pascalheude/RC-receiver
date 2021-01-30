/*****************************************************************/
/*                                                               */
/* RC Receiver                                                   */
/*                                                               */
/*****************************************************************/
#include <Arduino.h>
#include "standard.h"
#include "RC-receiver.h"
#include "data_ini.h"
#include "drone.h"
#include "led.h"

UNS32 pit_number;

static UNS32 loop_timer;

void setup(void)
{
    pit_number = 1;
    Serial.begin(115200);
#ifdef SPY
    Serial.begin(115200);
#endif
    initializeData();
    loop_timer = micros();
}

void loop(void)
{
    switchOnLed1();
    loop_timer = micros() + 1000 * PIT_PERIOD;
    manageDrone();
    pit_number++;
    if (F_no_reception == false)
    {
        switchOffLed1();
    }
    else
    {
    }
    while(micros() < loop_timer)
    {
    }
}