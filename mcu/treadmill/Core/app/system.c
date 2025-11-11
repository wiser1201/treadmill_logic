#include "system.h"
#include "hall.h"
#include "hd44780.h"
#include "tim1.h"
#include "tim3.h"
#include "time.h"
#include "lcd.h"
#include "fault.h"
#include "buttons.h"
#include "buzzer.h"
#include "main.h"
#include "motion.h"

typedef enum
{
    SS_STOP = 0,
    SS_START
} SystemState_e;

static SystemState_e systemState = SS_STOP;

static void drivers_init(void);
static void apps_init(void);
static void drivers_start(void);
static void drivers_stop(void);
static void drivers_reset(void);
static void buttons_handler(void);
static void lcd_update(void);
static void switch_state(void);

void system_init(void)
{
    drivers_init();
    apps_init();
}

void system_tick(void)
{
    while (systemState == SS_STOP)
    {
        buttons_handler();
        lcd_setSpeed(motion_getSpeed());
        buzzer_tickBlocked();
    }
    motion_softStart();
    tim3_reset();
    while (systemState == SS_START)
    {
        motion_tick();
        fault_check();
        // buttons_handler();
        // buzzer_tick();
        // lcd_update();
    }
}

void drivers_init(void)
{
    tim3_init();
    tim3_start(); // always must run
    hall_init();
    tim1_init();
    hd44780_init();
}

void apps_init(void)
{
    buttons_init();
    buzzer_init();
    lcd_init();
}

static void drivers_start(void)
{
    tim1_start();
}

static void drivers_stop(void)
{
    tim1_stop();
}

void drivers_reset(void)
{
    tim1_reset();
    hall_reset();
    fault_reset();
    motion_reset();
}

void buttons_handler(void)
{
    buttons_update();
    if (buttons_isPressed(BTN_SS))
    {
        switch_state();
    }
    else if (buttons_isPressed(BTN_DECR))
    {
        motion_decrSpeed();
    }
    else if (buttons_isPressed(BTN_INCR))
    {
        motion_incrSpeed();
    }
    if (buttons_anyIsPressed())
    {
        buzzer_beep(100);
    }
    buttons_clear();
}

void lcd_update(void)
{
    static int time_sec_prev = 0;
    static float distance = 0; 
    static float calories = 0;

    const int speed = motion_getSpeed();
    const int time_sec = time_s();    
    const float time_h = (time_sec - time_sec_prev)/60.f/60.f;
    time_sec_prev = time_sec;
    distance += time_h * speed;

    const int weight_kg = 70;
    const int met = speed;
    calories += met * weight_kg * time_h;
    
    lcd_setSpeed(speed);
    lcd_setCalories(calories);
    lcd_setDistance((int)distance);	
    lcd_setTime(time_sec);    
}

void switch_state(void)
{
    const SystemState_e targetState = !systemState;
    if (targetState)
    {
        drivers_start();
    }
    else
    {
        __disable_irq();
        drivers_stop();
        drivers_reset();
        tim3_saveState();
        __enable_irq();
    }
    systemState = targetState;
}