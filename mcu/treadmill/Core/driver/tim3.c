#include "tim3.h"
#include "main.h"

TIM_HandleTypeDef htim3;

volatile static uint32_t overf_cntr = 0;
static uint32_t overf_cntr_saved = 0;

void tim3_msp_init(void);
void tim3_msp_deinit(void);

void tim3_init(void)
{
    htim3.Instance = TIM3;
    htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.Prescaler = TIMER3_PRESC; // 500 kHz freq calibated, 2 us tick
    htim3.Init.Period = TIMER3_PERIOD; // 131 ms period
    htim3.Init.RepetitionCounter = 0;
    if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
    {
        Error_Handler();
    }
}

void tim3_start(void)
{
    if (HAL_TIM_Base_Start_IT(&htim3) != HAL_OK)
    {
        Error_Handler();
    }
}

void tim3_stop(void)
{
    HAL_TIM_Base_Stop_IT(&htim3);
}

void tim3_reset(void)
{
    if (overf_cntr_saved)
    {
        overf_cntr = overf_cntr_saved;
        overf_cntr_saved = 0;
    }
    else
    {
        overf_cntr = 0;
    }
}

void tim3_saveState(void)
{
    overf_cntr_saved = overf_cntr;
}

bool tim3_isActive(void)
{
    return htim3.Instance->CR1 & TIM_CR1_CEN_Msk;
}

void tim3_msp_init(void)
{
    __HAL_RCC_TIM3_CLK_ENABLE();

    HAL_NVIC_SetPriority(TIM3_IRQn, 15, 0);
    HAL_NVIC_EnableIRQ(TIM3_IRQn);
    TIM3->CR1 |= (1 << 2); // update only on overflow
}

void tim3_msp_deinit(void)
{
    __HAL_RCC_TIM3_CLK_DISABLE();
    HAL_NVIC_DisableIRQ(TIM3_IRQn);
}

void TIM3_IRQHandler(void)
{
    if (TIM3->SR & TIM_IT_UPDATE)
  	{
		++overf_cntr;
		TIM3->SR &= ~(TIM_IT_UPDATE);
	}
    else
    {
        Error_Handler();
    }
}

uint32_t tim3_overf_cntr(void)
{
    return overf_cntr;
}

uint32_t tim3_cntr(void)
{
    return TIM3->CNT;
}