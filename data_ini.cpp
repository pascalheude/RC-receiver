#include <Arduino.h>
#include "standard.h"
#include "data_ini.h"
#include "battery.h"
#include "drone.h"
#include "gps.h"
#include "led.h"
#include "mode.h"
#include "mpu6050.h"
#include "pid.h"
#include "nrf24l01.h"

void initializeData(void)
{
    initializeBattery();
    initializeDrone();
    initializeGps();
    initializeLed();
    initializeMode();
    initializeMpu6050();
    initializePid();
    initializeRadio();
}