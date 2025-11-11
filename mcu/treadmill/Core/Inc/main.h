#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "stm32f1xx_hal.h"

#define FLAG_IS_SET(reg, flag) ((reg) & (1 << (flag)))
#define SET_FLAG(reg, flag)    ((reg) |= (1 << (flag)))
#define CLEAR_FLAG(reg, flag)  ((reg) &= ~(1 << (flag)))
#define TOGGLE_FLAG(reg, flag) ((reg) ^= (1 << (flag)))

void Error_Handler(void);


#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
