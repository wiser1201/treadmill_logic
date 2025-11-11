#ifndef HD44780_H
#define HD44780_H

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
    DD_INPUT = 0,
    DD_OUTPUT
} DataDirection_e;

typedef enum
{
    HDP_RS,
    HDP_RW,
    HDP_ENABLE,
    HDP_DATA4 = 4,
    HDP_DATA5,
    HDP_DATA6,
    HDP_DATA7
} HD44780_Pin_e;

/**
 * @brief  Initializes driver. Before calling this function,
 *         lcd must initialize itself for > 10 ms.
 * @retval None
 */
void hd44780_init(void);

/**
 * @brief  Prints ASCII character.
 * @param  x: 0-15, position index on the 16x2 display
 * @param  y: 0-1, aposition index on the 16x2 display
 * @param  ch: character
 * @retval None
 */
void hd44780_print(const uint8_t x, const uint8_t y, const char ch);

/**
 * @brief  Prints a string in a x line.
 * @param  x: 0-15, position index on the 16x2 display
 * @param  y: 0-1, position index on the 16x2 display
 * @param  str: pointer to a string with '\0'
 * @retval None
 */
void hd44870_printLine(const uint8_t x, const uint8_t y, const char* str);

#endif // HD44780_H