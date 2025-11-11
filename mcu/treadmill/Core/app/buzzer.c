#include "buzzer.h"
#include "main.h"
#include "time.h"
#include <stdbool.h>

#define BUZZER_PORT GPIOB
#define BUZZER_PIN GPIO_PIN_5

static uint32_t timeout = 0;

void buzzer_init(void)
{
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitTypeDef gpio;
    gpio.Mode = GPIO_MODE_OUTPUT_PP;
    gpio.Pin = BUZZER_PIN;
    gpio.Pull = GPIO_PULLDOWN;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(BUZZER_PORT, &gpio);
}

void buzzer_tick(void)
{
    if (timeout)
    {
        if (time_ms() > timeout)
        {
            HAL_GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_RESET);
            timeout = 0;
        }
    }
}

void buzzer_tickBlocked(void)
{
    while (timeout)
    {
        if (time_ms() > timeout)
        {
            HAL_GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_RESET);
            timeout = 0;
        }
    }
}

bool buzzer_isActive(void)
{
    return timeout != 0;
}

void buzzer_beep(const int ms)
{
    timeout = time_ms() + ms;
    HAL_GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_SET);
}

void buzzer_beep_cycles(const uint32_t cycles)
{
    HAL_GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_SET);
    for (uint32_t i = 0; i < cycles; ++i) {}
    HAL_GPIO_WritePin(BUZZER_PORT, BUZZER_PIN, GPIO_PIN_RESET);
    for (uint32_t i = 0; i < cycles; ++i) {}
}