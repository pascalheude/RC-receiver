#include <Arduino.h>
#include "standard.h"
#include "drone.h"
#include "gps.h"
#include "led.h"
#include "mode.h"
#include "mpu6050.h"
#include "nrf24l01.h"

void initializeData(void)
{
    initializeDrone();
    initializeGps();
    initializeLed();
    initializeMode();
    initializeMpu6050();
    initializeRadio();
}