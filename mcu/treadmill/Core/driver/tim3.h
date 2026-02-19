#ifndef TIM3_H
#define TIM3_H

#include <stdint.h>
#include <stdbool.h>

#define TIMER3_FREQ 64000000UL
#define TIMER3_PRESC (640 - 1)
#define TIMER3_PERIOD (UINT16_MAX)
#define TIMER3_TICK_FREQ (TIMER3_FREQ / (TIMER3_PRESC + 1)) // 100 kHz
#define TIMER3_OVERF_FREQ (TIMER3_TICK_FREQ / (TIMER3_PERIOD + 1)) // 10 Hz
#define US_IN_SEC (1000UL * 1000UL)
#define TIMER3_TICK_US (US_IN_SEC / TIMER3_TICK_FREQ)
#define TIMER3_OVERF_US (TIMER3_TICK_US * TIMER3_PERIOD)

void tim3_init(void);
void tim3_start(void);
void tim3_stop(void);
void tim3_reset(void);
uint32_t tim3_overf_cntr(void);
uint32_t tim3_cntr(void);
bool tim3_isActive(void);
void tim3_saveState(void);

#endif // TIM3_H