#include <Arduino.h>
#include "standard.h"
#include "data_ini.h"
#include "battery.h"
#include "drone.h"
#include "esc.h"
#include "gps.h"
#include "led.h"
#include "mode.h"
#include "mpu6050.h"
#include "nrf24l01.h"
#include "pid.h"

void initializeData(void)
{
    initializeBattery();
    initializeDrone();
    initializeEsc();
    initializeGps();
    initializeLed();
    initializeMode();
    initializeMpu6050();
    initializeRadio();
    initializePid();
}