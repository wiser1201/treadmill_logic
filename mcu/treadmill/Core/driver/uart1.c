#include "uart1.h"
#include "main.h"
#include <stdio.h>

UART_HandleTypeDef huart1;

void uart1_init(void)
{
    huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.Mode = UART_MODE_TX;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	if (HAL_HalfDuplex_Init(&huart1) != HAL_OK)
	{
		Error_Handler();
	}
}

void uart1_check(void)
{
	printf("time: %d\n\r", (int)HAL_GetTick());
}

void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{
	__HAL_RCC_USART1_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	AFIO->MAPR |= AFIO_MAPR_USART1_REMAP_Msk;

	GPIO_InitTypeDef UART_Tx_Init = {0};
	UART_Tx_Init.Mode = GPIO_MODE_AF_OD;
	UART_Tx_Init.Pin = GPIO_PIN_6;
	UART_Tx_Init.Pull = GPIO_PULLUP;
	UART_Tx_Init.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &UART_Tx_Init);
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{
	__HAL_RCC_USART1_CLK_DISABLE();
	HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6);
}

int __io_putchar(int ch)
{
	HAL_UART_Transmit(&huart1, (uint8_t*)&ch, sizeof(char), 0xFF);
	return ch;
}