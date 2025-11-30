#include "time.h"
#include "tim3.h"


uint32_t time_us(void)
{
    uint32_t ovf1, ovf2, cnt;
    do {
        ovf1 = tim3_overf_cntr();
        cnt  = tim3_cntr();
        ovf2 = tim3_overf_cntr();
    } while (ovf1 != ovf2);

    return TIMER3_OVERF_US * ovf2 + (TIMER3_TICK_US * cnt);
}

uint32_t time_ms(void)
{
    return time_us() / 1000;
}

uint32_t time_s(void)
{
    return time_ms() / 1000;
}

uint32_t time_m(void)
{
    return time_s() / 60;
}