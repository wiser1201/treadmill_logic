#include "fault.h"
#include "hall.h"
#include "time.h"
#include "tim1.h"
#include "tim3.h"
#include "main.h"
#include "buzzer.h"
#include "motion.h"


#define HALL_MAX_SAMPLE_TIME_MS 500

static uint32_t hall_timeout = 0;

static void tim_check(void);
static void hall_check(void);
static void motion_check(void);

static void fault(const Driver_e target, const Fault_e cause);

void fault_check(void)
{
    tim_check();
    hall_check();
    motion_check();
}

void fault_reset(void)
{
    hall_timeout = 0;
}

void tim_check(void)
{
    if (!tim3_isActive())
    {
        fault(DR_TIMER, FAULT_ZERO_STATE);
    }
}

void hall_check(void)
{
    if (!hall_sampled())
    {
        if (hall_timeout)
        {
            if (time_ms() > hall_timeout)
            {
                fault(DR_HALL, FAULT_TIMEOUT);
            }
        }
        else
        {
            hall_timeout = time_ms() + HALL_MAX_SAMPLE_TIME_MS;
        }
    }
    else
    {
        hall_timeout = 0;
        hall_updateTime();
    }
}

void motion_check(void)
{
    if (motion_getCurrSpeed() > (motion_getSpeed() + 1))
    {
        fault(DR_MOTION, FAULT_OVERLOAD);
    }
}

void fault(const Driver_e target, const Fault_e cause)
{
    __disable_irq();
    __HAL_RCC_TIM1_FORCE_RESET();
    __HAL_RCC_TIM3_FORCE_RESET();

    const uint32_t hclk_freq = HAL_RCC_GetSysClockFreq();
    const int long_dev = 8;
    const int middle_dev = 32;
    const int short_dev = 64;
    
    while (1)
    {
        for (int i = 0; i < target; ++i)
        {
            buzzer_beep_cycles(hclk_freq / short_dev);
        }
        for (uint32_t i = 0; i < hclk_freq / middle_dev; ++i) {}
        for (int i = 0; i < cause; ++i)
        {
            buzzer_beep_cycles(hclk_freq / short_dev);
        }
        for (uint32_t i = 0; i < hclk_freq / long_dev; ++i) {}
    }
}