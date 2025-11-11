#ifndef BUZZER_H
#define BUZZER_H

#include <stdint.h>
#include <stdbool.h>

void buzzer_init(void);
void buzzer_tick(void);
void buzzer_tickBlocked(void);
void buzzer_beep(const int ms);
void buzzer_beep_cycles(const uint32_t cycles);
bool buzzer_isActive(void);

#endif // BUZZER_H