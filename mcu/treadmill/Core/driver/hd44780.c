#include "hd44780.h"

/* Instructions */
#define I_CLS (1 << 0)
#define I_RET_HOME (1 << 1)
#define I_MODE_SET (1 << 2)
#define I_ON_OFF (1 << 3)
#define I_SHIFT (1 << 4)
#define I_FUNC_SET (1 << 5)
#define I_SET_CG_ADDR (1 << 6)
#define I_SET_DD_ADDR (1 << 7)

/* Flags */
#define F_ID 1  // I_MODE_SET; INCR = 1, DECR = 0;
#define F_S 0   // I_MODE_SET; SHIFT DISAPLY = 1, NOP = 0;
#define F_D 2   // I_ON_OFF; DISPLAY ON = 1, DISPLAY OFF = 0;
#define F_C 1   // I_ON_OFF; CURSOR ON = 1, CURSOR OFF = 0;
#define F_B 0   // I_ON_OFF; BLINK ON = 1, BLINK OFF = 0;
#define F_SC 3  // I_SHIFT; DISPLAY MOVE = 1, CURSOR MOVE = 0;
#define F_RL 2  // I_SHIFT; RIGHT = 1, LEFT = 0;
#define F_DL 4  // I_FUNC_SET; DATA LENGTH 8 = 1, DL 4 = 0;
#define F_N 3   // I_FUNC_SET; LINES 2 = 1, LINES 1 = 0;
#define F_F 2   // I_FUNC_SET; FONT 5x10 = 1, FONT 5x8 = 0;
#define F_BF 7  // BUSY FLAG, read status reg

#define FLAG_IS_SET(reg, flag) ((reg) & (1 << (flag)))
#define SET_FLAG(reg, flag)    ((reg) |= (1 << (flag)))
#define CLEAR_FLAG(reg, flag)  ((reg) &= ~(1 << (flag)))
#define TOGGLE_FLAG(reg, flag) ((reg) ^= (1 << (flag)))

#define HIGH 1
#define LOW 0

/* Extern board-specific functions must be implemented in user code */
extern void ex_delay_us(const uint32_t us);
extern void ex_gpio_config(const HD44780_Pin_e pin, const DataDirection_e dir);
extern void ex_gpio_write(const HD44780_Pin_e pin, const bool level);
extern bool ex_gpio_read(const HD44780_Pin_e pin);


/* Communication protocol driver section */
#define NUM_OF_DL 8 
#define NUM_OF_USED_DL 4 // num of used data lines
#define PULSE_US 100
#define GAP_US (PULSE_US / 2)
#define PERIOD_US ((PULSE_US * 2 + GAP_US) * 2) // two pulses (4 + 4 bits data) + gap in-between

static DataDirection_e dataDir = DD_INPUT;

static void parallel_tx(const bool rs, const uint8_t data); // transmit
static uint8_t parallel_rx(const bool rs); // receive
static void parallel_cdd(void); // change data direction
static bool wait_for_unbusy_flag(void);

void parallel_tx(const bool rs, const uint8_t data)
{
    if (wait_for_unbusy_flag() == false)
    {
        return;
    }

    if (dataDir != DD_OUTPUT)
    {
        parallel_cdd();
    }

    ex_gpio_write(HDP_ENABLE, LOW);
    ex_gpio_write(HDP_RS, rs);
    ex_gpio_write(HDP_RW, LOW);

    const int msb = 7;
    const int numOfStages = NUM_OF_DL / NUM_OF_USED_DL;
    const int lsp = NUM_OF_DL / numOfStages;
    for (int stage = 0, bit_i = msb; stage < numOfStages; ++stage)
    {
        for (int pin_i = msb; pin_i >= lsp && bit_i >= 0; --pin_i, --bit_i)
        {
            ex_gpio_write((HD44780_Pin_e)pin_i, FLAG_IS_SET(data, bit_i));
        }
        ex_gpio_write(HDP_ENABLE, HIGH);
        ex_delay_us(PULSE_US);
        ex_gpio_write(HDP_ENABLE, LOW);
        ex_delay_us(GAP_US);
    }
}

uint8_t parallel_rx(const bool rs)
{
    if (rs)
    {
        if (wait_for_unbusy_flag() == false)
        {
            return 0xFF;
        }
    }

    if (dataDir != DD_INPUT)
    {
        parallel_cdd();
    }

    ex_gpio_write(HDP_ENABLE, LOW);
    ex_gpio_write(HDP_RS, rs);
    ex_gpio_write(HDP_RW, HIGH);

    uint8_t ret = 0;
    const int msb = 7;
    const int numOfStages = NUM_OF_DL / NUM_OF_USED_DL;
    const int lsp = NUM_OF_DL / numOfStages; // last pin
    for (int stage = 0, bit_i = msb; stage < numOfStages; ++stage)
    {
        ex_gpio_write(HDP_ENABLE, HIGH);
        ex_delay_us(PULSE_US);
        for (int pin_i = msb; pin_i >= lsp && bit_i >= 0; --pin_i, --bit_i)
        {
            const uint8_t signal = ex_gpio_read((HD44780_Pin_e)pin_i);
            ret |= (signal << bit_i);
        }
        ex_gpio_write(HDP_ENABLE, LOW);
        ex_delay_us(GAP_US);
    }
    return ret;
}

void parallel_cdd(void)
{
    DataDirection_e target_dir = !dataDir;

    for (int pin = 7; pin >= NUM_OF_USED_DL; --pin)
    {
        ex_gpio_config((HD44780_Pin_e)pin, target_dir);
    }
    dataDir = target_dir;
}

bool wait_for_unbusy_flag(void)
{
    ex_delay_us(2000);
    return true;
}
/* ~Communication protocol driver section end */

/* LCD driver section */
#define DISPLAY_X_LENGTH 16
#define DISPLAY_Y_LENGTH 2
#define DISPLAY_Y_ADDR_SHIFT 0x40

void hd44780_init(void)
{
    ex_gpio_config(HDP_RS, DD_OUTPUT);
    ex_gpio_config(HDP_RW, DD_OUTPUT);
    ex_gpio_config(HDP_ENABLE, DD_OUTPUT);
    dataDir = DD_OUTPUT; // it's input by default, it's just for swap
    parallel_cdd();     // now D4-D7 are initialized as inputs

    /* for some reason my module requires exactly 2 cycles 
    of initialization. It doesn't work with just 1 */
    for (int i = 0; i < 2; ++i)
    {
        uint8_t func_set = I_FUNC_SET;
        SET_FLAG(func_set, F_N);
        parallel_tx(LOW, func_set);

        uint8_t on_off = I_ON_OFF;
        SET_FLAG(on_off, F_D);
        // SET_FLAG(on_off, F_C);
        // SET_FLAG(on_off, F_B);
        parallel_tx(LOW, on_off);

        uint8_t mode_set = I_MODE_SET;
        SET_FLAG(mode_set, F_ID);
        parallel_tx(LOW, mode_set);

        uint8_t clear_display = I_CLS;
        parallel_tx(LOW, clear_display);

        uint8_t set_dd_addr = I_SET_DD_ADDR;
        parallel_tx(LOW, set_dd_addr);
    }
}

void hd44780_print(const uint8_t x, const uint8_t y, const char ch)
{
    if (x >= DISPLAY_X_LENGTH || y >= DISPLAY_Y_LENGTH)
    {
        return;
    }

    uint8_t addr = x + y * DISPLAY_Y_ADDR_SHIFT;
    parallel_tx(LOW, addr | I_SET_DD_ADDR);
    parallel_tx(HIGH, ch);
}

void hd44870_printLine(const uint8_t x, const uint8_t y, const char *str)
{
    if (x >= DISPLAY_X_LENGTH || y >= DISPLAY_Y_LENGTH)
    {
        return;
    }

    uint8_t addr = x + y * DISPLAY_Y_ADDR_SHIFT;
    parallel_tx(LOW, addr | I_SET_DD_ADDR);
    uint8_t mode_set = I_MODE_SET;
    parallel_tx(LOW, mode_set);

    for (const char* ptr = str; *ptr != '\0'; ++ptr)
    {
        parallel_tx(HIGH, *ptr);
    }
}
/* LCD driver section end */