#include "buttons.h"
#include "main.h"
#include "time.h"

#define VALID_STATE_DELAY 10

#define BTNS_MSK ((1 << BTN_SS) | (1 << BTN_INCR) | (1 << BTN_DECR))

static uint32_t btns_state_msk = BTNS_MSK; // reset logic state
static uint32_t btns_out = BTNS_MSK; // reset logic state

void buttons_init(void)
{
    GPIO_InitTypeDef btns;
    btns.Mode = GPIO_MODE_INPUT;
    btns.Pin = BTNS_MSK;
    btns.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &btns);
}

void buttons_update(void)
{
    static uint32_t changed_time = 0;
    static uint32_t btns_prev_state = 0;
    const uint32_t btns_curr_state = GPIOB->IDR & BTNS_MSK;

    if (btns_state_msk != btns_curr_state)
    {
        if (btns_curr_state == btns_prev_state)
        {
            if (time_ms() > (changed_time + VALID_STATE_DELAY))
            {
                btns_state_msk = btns_curr_state;
                btns_out = btns_state_msk;
            }
        }
        else
        {
            btns_prev_state = btns_curr_state;
            changed_time = time_ms();
        }
    }
}

bool buttons_isPressed(const Button_e btn)
{
    return FLAG_IS_SET(btns_out, btn) == false;
}

bool buttons_anyIsPressed(void)
{
    return (btns_out & BTNS_MSK) != BTNS_MSK;
}

void buttons_clear(void)
{
    btns_out = BTNS_MSK;
}