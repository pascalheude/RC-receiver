#include <Arduino.h>
#include "standard.h"
#include "mapping.h"
#include "battery.h"
#include "RC-receiver.h"

BOOLEAN F_low_battery;
REAL32 battery_voltage;

static UNS32 low_battery_time;

void initializeBattery(void)
{
    F_low_battery = false;
    // Battery = 11.1V
    // V divider = 2.5
    // 11.1V (battery) => 4.44V (pin BATTERY) = 908 (ADC) => 100%
    battery_voltage = (REAL32)analogRead(BATTERY) / (REAL32)9.08f;
}

void monitorBatteryVoltage(void)
{
    // Filter to reduce noise : 0.0088106 = 0.08 / 9.08
    battery_voltage = ((REAL32)0.92f * battery_voltage) + ((REAL32)0.0088106f * (REAL32)analogRead(BATTERY));
    if (battery_voltage < (REAL32)50.0f)
    {
        if (low_battery_time == 0)
        {
            low_battery_time = pit_number;
        }
        else if ((pit_number - low_battery_time) >= (5000/PIT_PERIOD))
        {
             F_low_battery = true;
        }
        else
        {
        }       
    }
    else
    {
        low_battery_time = 0;
        F_low_battery = false;
    }
}