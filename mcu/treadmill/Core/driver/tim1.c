#include "tim1.h"
#include "main.h"

TIM_HandleTypeDef htim1;

void tim1_mspInit(void);
void tim1_mspDeInit(void);

void tim1_init(void)
{
    htim1.Instance = TIM1;
    htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim1.Init.Prescaler = TIMER1_PRESC;
    htim1.Init.Period = TIMER1_PERIOD;
    htim1.Init.RepetitionCounter = 0;
    if (HAL_TIM_OnePulse_Init(&htim1, TIM_OPMODE_SINGLE) != HAL_OK)
    {
        Error_Handler();
    }
    if (HAL_TIM_OC_Init(&htim1) != HAL_OK)
    {
        Error_Handler();
    }

    TIM_SlaveConfigTypeDef sctim1 = {0};
    sctim1.InputTrigger = TIM_TS_TI1FP1;
    sctim1.SlaveMode = TIM_SLAVEMODE_TRIGGER;
    sctim1.TriggerFilter = 3;
    sctim1.TriggerPolarity = TIM_TRIGGERPOLARITY_FALLING;
    sctim1.TriggerPrescaler = TIM_TRIGGERPRESCALER_DIV1;
    if (HAL_TIM_SlaveConfigSynchro(&htim1, &sctim1) != HAL_OK)
    {
        Error_Handler();
    }

    TIM_OC_InitTypeDef octim1 = {0};
    octim1.OCFastMode = TIM_OCFAST_ENABLE;
    octim1.OCIdleState = TIM_OCIDLESTATE_RESET;
    octim1.OCMode = TIM_OCMODE_PWM2;
    octim1.OCPolarity = TIM_OCPOLARITY_HIGH;
    octim1.Pulse = htim1.Init.Period + 1;
    if (HAL_TIM_OC_ConfigChannel(&htim1, &octim1, TIM_CHANNEL_2) != HAL_OK)
    {
        Error_Handler();
    }
}

void tim1_start(void)
{
    if (HAL_TIM_OnePulse_Start(&htim1, TIM_CHANNEL_2) != HAL_OK)
    {
        Error_Handler();
    }
}

void tim1_stop(void)
{
    HAL_TIM_OnePulse_Stop(&htim1, TIM_CHANNEL_2);
}

void tim1_reset(void)
{
    tim1_stop();
    __HAL_RCC_TIM1_FORCE_RESET();
    __HAL_RCC_TIM1_RELEASE_RESET();
    tim1_init();
}

void tim1_setPeriod(const uint32_t period)
{
    htim1.Instance->ARR = period;
    htim1.Instance->CCR2 = period - TIMER1_PULSE_US;
}

uint32_t tim1_getPeriod(void)
{
    return htim1.Instance->ARR;
}

void tim1_mspInit(void)
{
    __HAL_RCC_TIM1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef tim1_gpio = {0};
    tim1_gpio.Mode = GPIO_MODE_AF_INPUT;
    tim1_gpio.Pin = GPIO_PIN_8;
    tim1_gpio.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &tim1_gpio);

    tim1_gpio.Mode = GPIO_MODE_AF_PP;
    tim1_gpio.Pin = GPIO_PIN_9;
    tim1_gpio.Pull = GPIO_NOPULL;
    tim1_gpio.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &tim1_gpio);
}

void tim1_mspDeInit(void)
{
    __HAL_RCC_TIM1_CLK_DISABLE();
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_8 | GPIO_PIN_9);
}