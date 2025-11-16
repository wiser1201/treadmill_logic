#include "hall.h"
#include "main.h"
#include "time.h"


#define NUM_OF_MAGNETS 1
#define HALL_RPM_MAX 3000
#ifndef SEC_IN_US
#define SEC_IN_US (1000UL * 1000UL)
#endif

static volatile bool irq_blocked = false;
static uint32_t rpm_prev = 0;
static volatile uint32_t first_time = 0;
static volatile uint32_t second_time = 0;

void hall_irq(void);
static void record_time(void);
static uint32_t calculateRPM(const uint32_t firstTimeMark, const uint32_t secondTimeMark);

void hall_init(void)
{
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitTypeDef gpio_hall;
    gpio_hall.Mode = GPIO_MODE_IT_FALLING;
    gpio_hall.Pin = GPIO_PIN_4;
    gpio_hall.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &gpio_hall);

    HAL_NVIC_SetPriority(EXTI4_IRQn, 10, 0);
    HAL_NVIC_EnableIRQ(EXTI4_IRQn);
}

bool hall_sampled(void)
{
    if (first_time && second_time)
    {
        return true;
    }
    return false;
}

uint32_t hall_getRPM(void)
{
    if (first_time && second_time)
    {
        irq_blocked = true;
        uint32_t ret = calculateRPM(first_time, second_time);
        rpm_prev = ret;
        irq_blocked = false;
        return ret;
    }
    return 0;
}

uint32_t hall_getPrevRPM(void)
{
    return rpm_prev;
}

void hall_updateTime(void)
{
    first_time = second_time;
    second_time = 0;
}

void hall_reset(void)
{
    irq_blocked = false;
    rpm_prev = first_time = second_time = 0;
}

uint32_t calculateRPM(const uint32_t firstTimeMark, const uint32_t secondTimeMark)
{
    if (firstTimeMark == 0 || secondTimeMark <= firstTimeMark)
    {
        return 0;
    }

    const uint32_t difference = secondTimeMark - firstTimeMark;
    const uint32_t revolution_time_us = difference * NUM_OF_MAGNETS;
    const uint32_t result = (60UL * SEC_IN_US) / revolution_time_us;
    return result;
}

void record_time(void)
{
    if (first_time)
    {
        if (second_time)
        {
            first_time = second_time;
        }
        second_time = time_us();
    }
    else
    {
        first_time = time_us();
    }
}

void hall_irq(void)
{
    if (irq_blocked == false)
    {
        record_time();
    }    
}