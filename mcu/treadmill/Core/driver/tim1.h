#ifndef TIM1_H
#define TIM1_H

#define TIMER1_FREQ 64000000UL
#define TIMER1_PRESC (64 - 1)
#define TIMER1_PERIOD (9100 - 300)

void tim1_init(void);
void tim1_start(void);
void tim1_stop(void);
void tim1_reset(void);

#endif // TIM1_H