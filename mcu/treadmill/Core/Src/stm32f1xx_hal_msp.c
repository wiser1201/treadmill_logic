#include "main.h"


extern void tim1_mspInit(void);
extern void tim1_mspDeInit(void);

void HAL_MspInit(void)
{
	__HAL_RCC_AFIO_CLK_ENABLE();
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_AFIO_REMAP_SWJ_NOJTAG();
}

void HAL_TIM_OnePulse_MspInit(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM1)
	{
		tim1_mspInit();
	}
}

void HAL_TIM_OnePulse_MspDeInit(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM1)
	{
		tim1_mspDeInit();
	}
}