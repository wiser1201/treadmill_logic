#ifndef BUTTONS_H
#define BUTTONS_H

#include <stdbool.h>

typedef enum
{
    BTN_NONE,
    BTN_SS = 6,
    BTN_INCR = 7,
    BTN_DECR = 8
} Button_e;

void buttons_init(void);
void buttons_update(void);
bool buttons_isPressed(const Button_e btn);
bool buttons_anyIsPressed(void);
void buttons_clear(void);

#endif // BUTTONS_H