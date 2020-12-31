#include <Arduino.h>
#include "led.h"
#include "mapping.h"

void initializeLed(void)
{
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    pinMode(LED3, OUTPUT);
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
}

void blinkLed1(void)
{

}

void blinkLed2(void)
{

}

void blinkLed3(void)
{

}

void switchOnLed1(void)
{
    digitalWrite(LED1, HIGH);
}

void switchOnLed2(void)
{
    digitalWrite(LED2, HIGH);
}

void switchOnLed3(void)
{
    digitalWrite(LED3, HIGH);
}