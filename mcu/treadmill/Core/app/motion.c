#include "motion.h"
#include "tim1.h"
#include "hall.h"
#include "time.h"
#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#define SPEED_MAX 12
#define SPEED_MIN 4
#define SOFT_START_DELAY 700
#define SINE_PERIOD 20000
#define TRIG_LATENCY 300
#define DEAD_TIME 500
#define MIN_SPEED_SHIFT 1500
#define PERIOD_MAX ((SINE_PERIOD / 2) - TRIG_LATENCY - DEAD_TIME - MIN_SPEED_SHIFT)
#define PERIOD_MIN ((SINE_PERIOD / 2) * 0.4)

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

static int targ_speed = SPEED_MIN;
static float curr_speed = 0;
static PID_Data pid_data = 
{
    .Kp = 200.f,
    .Ki = 2000.f,
    .min = 0,
    .max = PERIOD_MAX - PERIOD_MIN,
    .hyst = 0.1f
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
        int period = PERIOD_MAX + (-pid_calc(&pid_data));
        if (tim1_getPeriod() != period)
        {
            tim1_setPeriod(period);
        }
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
    tim1_setPeriod(PERIOD_MAX);
    uint32_t timeout = time_ms() + SOFT_START_DELAY;
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
    if ((targ_speed + 1) <= SPEED_MAX)
    {
        ++targ_speed;
    }
}

void motion_decrSpeed(void)
{
    if ((targ_speed - 1) >= SPEED_MIN)
    {
        --targ_speed;
    }
}