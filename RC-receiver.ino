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

UNS32 pit_number;

UNS32 loop_timer;

void setup(void)
{
    pit_number = 1;
#ifdef SPY
    Serial.begin(115200);
#endif
    initializeData();
    loop_timer = micros();
}

void loop(void)
{
    manageDrone();
    while((micros() - loop_timer) < (PIT_PERIOD * 1000))
    {
    }
    loop_timer = micros();
    pit_number++;
}