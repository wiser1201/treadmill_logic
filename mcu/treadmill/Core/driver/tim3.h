#ifndef TIM3_H
#define TIM3_H

#include <stdint.h>
#include <stdbool.h>

#define TIMER3_FREQ 64000000UL
#define TIMER3_PRESC (129 - 1)
#define TIMER3_PERIOD (0xFFFF)
#define TIMER3_TICK_FREQ (TIMER3_FREQ / (TIMER3_PRESC + 1)) // 100 kHz
#define TIMER3_OVERF_FREQ (TIMER3_TICK_FREQ / (TIMER3_PERIOD + 1)) // 10 Hz
#define SEC_IN_US (1000UL * 1000UL)
#define TIMER3_TICK_US (SEC_IN_US / TIMER3_TICK_FREQ)
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