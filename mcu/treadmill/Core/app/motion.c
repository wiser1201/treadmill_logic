#include "motion.h"
#include "tim1.h"
#include "main.h"
#include "hall.h"
#include "time.h"
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define MAX_SPEED 12
#define MIN_SPEED 4
#define SOFT_START_DELAY 1000

typedef struct
{
    const float Kp;
    const float Ki;
    const float Kd;
    float i_pr;
    float error_pr;
    float error;
    const int min;
    const int max;
    const float hyst;
} PID_Data;

static int targ_speed = MIN_SPEED;
static float curr_speed = 0;
static PID_Data pid_data = 
{
    .Kp = 2.2f,
    .Ki = 25.f,
    .min = 0,
    .max = 550,
    .hyst = 0.3f
};

static int clamp(int val, int min, int max);
static int pid_calc(PID_Data* data);

void motion_tick(void)
{
    const float roller_diam = 0.05023;
    const float time_const = 0.06f;
    const uint32_t rpm = hall_sampled() ? hall_getRPM() : hall_getPrevRPM();
    curr_speed = M_PI * roller_diam * rpm * time_const;
    pid_data.error = targ_speed - curr_speed;
    if (abs(pid_data.error) > pid_data.hyst)
    {
        int cc = pid_calc(&pid_data);
        TIM1->CCR1 = cc;
    }
}

void motion_reset(void)
{
    curr_speed = 0;
    pid_data.error = 0;
    pid_data.error_pr = 0;
    pid_data.i_pr = 0;
}

void motion_softStart(void)
{
    pid_data.i_pr = (TIM1->ARR + 1) * 0.1f;
    TIM1->CCR1 = pid_data.i_pr;
    const uint32_t timeout = time_ms() + SOFT_START_DELAY;
    while (time_ms() < timeout) {}
}

int pid_calc(PID_Data* data)
{
    const int tick_us = 10;
    const float sample_period_sec = tick_us / 1000000.f;
    float p_part = data->Kp * data->error;
    float i_part = data->i_pr + data->Ki * sample_period_sec * data->error;
    int actuating_signal = p_part + i_part;
    int clamped = clamp(actuating_signal, data->min, data->max);

    if (actuating_signal < data->min)
    {
        if (data->error > 0)
        {
            data->i_pr = i_part;
        }
    }
    else if (actuating_signal > data->max)
    {
        if (data->error < 0)
        {
            data->i_pr = i_part;
        }
    }
    else
    {
        data->i_pr = i_part;
    }
    
    return clamped;
}

int clamp(int val, int min, int max)
{
    return val < min ? min : val > max ? max : val;
}

int motion_getSpeed(void)
{
    return targ_speed;
}

int motion_getCurrSpeed(void)
{
    return curr_speed;
}

void motion_incrSpeed(void)
{
    if ((targ_speed + 1) <= MAX_SPEED)
    {
        ++targ_speed;
    }
}

void motion_decrSpeed(void)
{
    if ((targ_speed - 1) >= MIN_SPEED)
    {
        --targ_speed;
    }
}