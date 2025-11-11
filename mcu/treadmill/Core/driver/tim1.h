#ifndef TIM1_H
#define TIM1_H

#define TIMER1_FREQ 64000000UL
#define TIMER1_PRESC (64 - 1)
#define TIMER1_PERIOD (9100 - 300)
#define TIMER1_PULSE_US 100

void tim1_init(void);
void tim1_start(void);
void tim1_stop(void);
void tim1_reset(void);
void tim1_setPeriod(const uint32_t period);
uint32_t tim1_getPeriod(void);

#endif // TIM1_H