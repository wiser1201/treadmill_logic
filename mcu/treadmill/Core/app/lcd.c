#include "lcd.h"
#include <stdbool.h>
#include <stdint.h>
#include "hd44780.h"
#include "main.h"
#include "time.h"
#include <stdlib.h>
#include <string.h>

#define C_POS 2
#define S_POS 5
#define MIN_POS 8
#define SEC_POS 11
#define D_POS 15

void ex_gpio_config(const HD44780_Pin_e pin, const DataDirection_e dir);
void ex_gpio_write(const HD44780_Pin_e pin, bool level);
bool ex_gpio_read(const HD44780_Pin_e pin);
void ex_delay_us(const uint32_t us);
static uint32_t getPinByEnum(const HD44780_Pin_e pin);

static void str_reverse(char* str);
static void str_twoZeros(char* str); // if target value == 0, we need "00" instead of "0"

void lcd_init(void)
{
    const char* first_line = " D    T    S  C "; //" C  S    T    D "; 
    const char* second_line = "000 00:00 00 000"; //"000 00 00:00 000"; 
    hd44870_printLine(15, 0, first_line);
    hd44870_printLine(15, 1, second_line);
}

void lcd_setCalories(const int calories)
{
	static int calor_prev = 0;
	if (calories == calor_prev) return;

    char buff[4];   // 3-digits + '\0'
    itoa(calories, buff, 10);
	str_reverse(buff);
    hd44870_printLine(C_POS, 1, buff);
	calor_prev = calories;
}

void lcd_setSpeed(const int kmh)
{
	static int speed_prev = 0;
	if (kmh == speed_prev) return;

    char buff[3];   // 2-digits + '\0'
    itoa(kmh, buff, 10);
	str_reverse(buff);
	str_twoZeros(buff);
    hd44870_printLine(S_POS, 1, buff);
	speed_prev = kmh;
}

void lcd_setTime(const int sec)
{
	static int time_prev = 0;
	if (sec == time_prev) return;

    char buff[3];   // 2-digits + '\0'
    int m = sec / 60;
    int s = sec % 60;

    itoa(m, buff, 10);
	str_reverse(buff);
    hd44870_printLine(MIN_POS, 1, buff);
    itoa(s, buff, 10);
	str_reverse(buff);
	str_twoZeros(buff);
    hd44870_printLine(SEC_POS, 1, buff);
	time_prev = sec;
}

void lcd_setDistance(const int km)
{
	static int dist_prev = 0;
	if (km == dist_prev) return;

    char buff[3];   // 2-digits + '\0'
    itoa(km, buff, 10);
	str_reverse(buff);
    hd44870_printLine(D_POS, 1, buff);
	dist_prev = km;
}

void str_reverse(char* str)
{
	const int len = strlen(str);
	for (int i = 0; i < len / 2; ++i)
    {
        char tmp = str[i];
        str[i] = str[len - 1 - i];
        str[len - 1 - i] = tmp;
    }
}

void str_twoZeros(char* str)
{
	if (strlen(str) == 1)
	{
		str[1] = '0';
		str[2] = '\0';
	}
}

void ex_gpio_config(const HD44780_Pin_e pin, const DataDirection_e dir)
{
	GPIO_InitTypeDef gpio = {0};
	gpio.Mode = (int)dir;
	gpio.Pin = getPinByEnum(pin);
	gpio.Pull = GPIO_NOPULL;
	gpio.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_DeInit(GPIOA, gpio.Pin);
	HAL_GPIO_Init(GPIOA, &gpio);
}

void ex_gpio_write(const HD44780_Pin_e pin, bool level)
{
	HAL_GPIO_WritePin(GPIOA, getPinByEnum(pin), level);
}

bool ex_gpio_read(const HD44780_Pin_e pin)
{
	return HAL_GPIO_ReadPin(GPIOA, getPinByEnum(pin));
}

void ex_delay_us(const uint32_t us)
{
	const uint32_t timeout = time_us() + us;
    while (time_us() < timeout) {}    
}

uint32_t getPinByEnum(const HD44780_Pin_e pin)
{
	uint32_t ret = 0;
	switch (pin)
	{
	case HDP_RS:
		ret = GPIO_PIN_1;
		break;
	case HDP_RW:
		ret = GPIO_PIN_2;
		break;
	case HDP_ENABLE:
		ret = GPIO_PIN_3;
		break;
	case HDP_DATA4:
		ret = GPIO_PIN_4;
		break;
	case HDP_DATA5:
		ret = GPIO_PIN_5;
		break;
	case HDP_DATA6:
		ret = GPIO_PIN_6;
		break;
	case HDP_DATA7:
		ret = GPIO_PIN_7;
		break;
	}
	return ret;
}