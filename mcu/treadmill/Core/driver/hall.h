#ifndef HALL_H
#define HALL_H

#include <stdint.h>
#include <stdbool.h>

void hall_init(void);
void hall_reset(void);
bool hall_sampled(void);
uint32_t hall_getRPM(void);
uint32_t hall_getPrevRPM(void);
void hall_updateTime(void);

#endif // HALL_H