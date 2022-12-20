#include <Arduino.h>
#include "standard.h"
#include "pid.h"
#include "drone.h"
#include "esc.h"
#include "mathf.h"
#include "mpu6050.h"
#include "spy.h"

REAL32 set_point[3];
REAL32 error[3];
REAL32 error_sum[3];
REAL32 error_delta[3];
REAL32 error_mem[3];

//                     YAW   PITCH ROLL
const REAL32 Kp[3] = { 4.0f, 1.3f, 1.3f };
const REAL32 Ki[3] = { 0.0f, 0.0f, 0.0f };
const REAL32 Kd[3] = { 0.0f, 0.0f, 0.0f };


#define PID_MIN (REAL32)-400.0f
#define PID_MAX (REAL32)400.0f

void initializePid(void)
{
    set_point[ROLL] = (REAL32)0.0f;
    set_point[PITCH] = (REAL32)0.0f;
    set_point[YAW] = (REAL32)0.0f;
    error_delta[ROLL] = (REAL32)0.0f;
    error_delta[PITCH] = (REAL32)0.0f;
    error_delta[YAW] = (REAL32)0.0f;
    resetPid();
}

void resetPid(void)
{
    error[ROLL] = (REAL32)0.0f;
    error[PITCH] = (REAL32)0.0f;
    error[YAW] = (REAL32)0.0f;
    error_sum[ROLL] = (REAL32)0.0f;
    error_sum[PITCH] = (REAL32)0.0f;
    error_sum[YAW] = (REAL32)0.0f;
    error_mem[ROLL] = (REAL32)0.0f;
    error_mem[PITCH] = (REAL32)0.0f;
    error_mem[YAW] = (REAL32)0.0f;
}

REAL32 calculateSetPoint(REAL32 angle, REAL32 command)
{
    REAL32 level_adjust;
    REAL32 set_point;

    level_adjust = (REAL32)15.0f * angle;
    if (command > (REAL32)1508.0f)
    {
        set_point = command - (REAL32)1508.0;
    }
    else if (command < (REAL32)1492.0f)
    {
        set_point = command - (REAL32)1492.0;
    }
    else
    {
        set_point = (REAL32)0.0f;
    }
    set_point -= level_adjust;
    set_point /= (REAL32)3.0f;
    return(set_point);
}

void calculateError(void)
{
    // Calculate the set points
    if (throttle > (REAL32)1050.0f)
    {
        set_point[YAW] = calculateSetPoint((REAL32)0.0f, yaw);
    }
    else
    {
        set_point[YAW]= (REAL32)0.0f;
    }
    set_point[PITCH] = calculateSetPoint(measure[PITCH], pitch);
    set_point[ROLL] = calculateSetPoint(measure[ROLL], roll);
    // Calculate the errors
    error[YAW] = angular_motion[YAW] - set_point[YAW];
    error[PITCH] = angular_motion[PITCH] - set_point[PITCH];
    error[ROLL] = angular_motion[ROLL] - set_point[ROLL];
    // Calculate the sum of errors
    error_sum[YAW] += error_sum[YAW];
    error_sum[PITCH] += error_sum[PITCH];
    error_sum[ROLL] += error_sum[ROLL];
    // Limit the sum of errors
    error_sum[YAW] = LIMIT(error_sum[YAW], PID_MIN / Ki[YAW], PID_MAX / Ki[YAW]);
    error_sum[PITCH] = LIMIT(error_sum[PITCH], PID_MIN / Ki[PITCH], PID_MAX / Ki[PITCH]);
    error_sum[ROLL] = LIMIT(error_sum[ROLL], PID_MIN / Ki[ROLL], PID_MAX / Ki[ROLL]);
    // Calculate the delta of errors
    error_delta[YAW] = error[YAW] - error_mem[YAW];
    error_delta[PITCH] = error[PITCH] - error_mem[PITCH];
    error_delta[ROLL] = error[ROLL] - error_mem[ROLL];
    // Memorize current errors
    error_mem[YAW] = error[YAW];
    error_mem[PITCH] = error[PITCH];
    error_mem[ROLL] = error[ROLL];
}

void performPid(void)
{
    REAL32 yaw_pid;
    REAL32 pitch_pid;
    REAL32 roll_pid;

    calculateError();
    if (throttle > (REAL32)1012.0f)
    {
        yaw_pid = Kp[YAW] * error[YAW] + Ki[YAW] * error_sum[YAW] + Kd[YAW] * error_delta[YAW];
        yaw_pid = LIMIT(yaw_pid, PID_MIN, PID_MAX);
        pitch_pid = Kp[PITCH] * error[PITCH] + Ki[PITCH] * error_sum[PITCH] + Kd[PITCH] * error_delta[PITCH];
        pitch_pid = LIMIT(pitch_pid, PID_MIN, PID_MAX);
        roll_pid = Kp[ROLL] * error[ROLL] + Ki[ROLL] * error_sum[ROLL] + Kd[ROLL] * error_delta[ROLL];
        roll_pid = LIMIT(roll_pid, PID_MIN, PID_MAX);
        lf_esc_value = (UNS16)(throttle - roll_pid - pitch_pid + yaw_pid);
        rf_esc_value = (UNS16)(throttle + roll_pid - pitch_pid - yaw_pid);
        lr_esc_value = (UNS16)(throttle - roll_pid + pitch_pid - yaw_pid);
        rr_esc_value = (UNS16)(throttle + roll_pid + pitch_pid + yaw_pid);
    }
    else
    {
        lf_esc_value = (UNS16)(throttle);
        rf_esc_value = (UNS16)(throttle);
        lr_esc_value = (UNS16)(throttle);
        rr_esc_value = (UNS16)(throttle);
    }
#ifdef SPY_PID
    Serial.print("error[YAW]=");
    Serial.print(error[YAW], 4);
    Serial.print(",error[PITCH]=");
    Serial.print(error[PITCH], 4);
    Serial.print(",error[ROLL]=");
    Serial.print(error[ROLL], 4);
    Serial.print(",yaw_pid=");
    Serial.print(yaw_pid, 4);
    Serial.print(",pitch_pid=");
    Serial.print(pitch_pid, 4);
    Serial.print(",roll_pid=");
    Serial.println(roll_pid, 4);
#endif
}