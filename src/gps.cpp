#include <Arduino.h>
#include "gps.h"
#include "mapping.h"

void initializeGps(void)
{
    pinMode(GPS_EN, OUTPUT);
    digitalWrite(GPS_EN, LOW);
}