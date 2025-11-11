#ifndef FAULT_H
#define FAULT_H

typedef enum
{
    DR_NONE,
    DR_HALL,
    DR_TIMER,
    DR_MOTION
} Driver_e;

typedef enum
{
    FAULT_TIMEOUT = 1,
    FAULT_OVERLOAD,
    FAULT_ZERO_STATE
} Fault_e;

void fault_check(void);
void fault_reset(void);

#endif // FAULT_H