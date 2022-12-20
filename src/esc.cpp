#include <Arduino.h>
#include <Servo.h>
#include "standard.h"
#include "esc.h"
#include "mapping.h"
#include "mathf.h"
#include "RC-receiver.h"

UNS16 lf_esc_value;
UNS16 rf_esc_value;
UNS16 lr_esc_value;
UNS16 rr_esc_value;

#define STOP_ESC 1000
#define MIN_ESC 1100
#define MAX_ESC 2000

static UNS8 esc_state;
static UNS32 esc_time;
static Servo lf_esc;
static Servo rf_esc;
static Servo lr_esc;
static Servo rr_esc;

void initializeEsc(void)
{
    esc_state = 0;
    esc_time = 0;
    lf_esc.attach(CH1);
    rf_esc.attach(CH2);
    lr_esc.attach(CH3);
    rr_esc.attach(CH4);
    stopEsc();
}

void driveEsc(void)
{
    lf_esc.writeMicroseconds(LIMIT(lf_esc_value, MIN_ESC, MAX_ESC));
    rf_esc.writeMicroseconds(LIMIT(rf_esc_value, MIN_ESC, MAX_ESC));
    lr_esc.writeMicroseconds(LIMIT(lr_esc_value, MIN_ESC, MAX_ESC));
    rr_esc.writeMicroseconds(LIMIT(rr_esc_value, MIN_ESC, MAX_ESC));
}

void stopEsc(void)
{
    lf_esc_value = STOP_ESC;
    lf_esc.writeMicroseconds(STOP_ESC);
    rf_esc_value = STOP_ESC;
    rf_esc.writeMicroseconds(STOP_ESC);
    lr_esc_value = STOP_ESC;
    lr_esc.writeMicroseconds(STOP_ESC);
    rr_esc_value = STOP_ESC;
    rr_esc.writeMicroseconds(STOP_ESC);
}