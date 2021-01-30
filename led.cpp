#include <Arduino.h>
#include "standard.h"
#include "led.h"
#include "mapping.h"
#include "RC-receiver.h"

static UNS8 led1_state;
static UNS8 led2_state;
static UNS8 led3_state;
static UNS32 led1_time;
static UNS32 led2_time;
static UNS32 led3_time;

void initializeLed(void)
{
    led1_state = LOW;
    led2_state = LOW;
    led3_state = LOW;
    led1_time = 0;
    led2_time = 0;
    led3_time = 0;
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    pinMode(LED3, OUTPUT);
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
}

void blinkLed1(UNS32 time_out)
{
    if ((pit_number - led1_time) > (time_out/PIT_PERIOD))
    {
        led1_time = pit_number;
        if (led1_state == LOW)
        {
            led1_state = HIGH;
        }
        else
        {
            led1_state = LOW;
        }
        digitalWrite(LED1, led1_state);
    }
    else
    {
    }    
}

void blinkLed2(UNS32 time_out)
{
    if ((pit_number - led2_time) > (time_out/PIT_PERIOD))
    {
        led2_time = pit_number;
        if (led2_state == LOW)
        {
            led2_state = HIGH;
        }
        else
        {
            led2_state = LOW;
        }
        digitalWrite(LED2, led2_state);
    }
    else
    {
    }    
}

void blinkLed3(UNS32 time_out)
{
    if ((pit_number - led3_time) > (time_out/PIT_PERIOD))
    {
        led3_time = pit_number;
        if (led3_state == LOW)
        {
            led3_state = HIGH;
        }
        else
        {
            led3_state = LOW;
        }
        digitalWrite(LED3, led3_state);
    }
    else
    {
    }    
}

void switchOffLed1(void)
{
    led1_time = 0;
    led1_state = LOW;
    digitalWrite(LED1, LOW);
}

void switchOnLed1(void)
{
    led1_time = 0;
    led1_state = HIGH;
    digitalWrite(LED1, HIGH);
}

void switchOffLed2(void)
{
    led2_time = 0;
    led2_state = LOW;
    digitalWrite(LED2, LOW);
}

void switchOnLed2(void)
{
    led2_time = 0;
    led2_state = HIGH;
    digitalWrite(LED2, HIGH);
}

void switchOffLed3(void)
{
    led3_time = 0;
    led3_state = LOW;
    digitalWrite(LED3, LOW);
}

void switchOnLed3(void)
{
    led3_time = 0;
    led3_state = HIGH;
    digitalWrite(LED3, HIGH);
}