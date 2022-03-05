/**
 * @file timer_handler.c
 *
 * @brief Init TIMER0 to control buzzer with PWM signal.
 *
 * @author Johannes Ehala, ProLab.
 * @license MIT
 *
 * Copyright ProLab, TTÜ. 15 January 2022
 */

#include "em_cmu.h"
#include "em_timer.h"

#include "timer_handler.h"

/**
 * @brief Gpio_init function.
 * Initialize GPIO and set up pins for leds
 */
void led_gpio_init()
{
    // Enable clock for GPIO module
    CMU_ClockEnable(cmuClock_GPIO, true);

    // Set up LED pins
    GPIO_PinModeSet(LED_RED_PORT, LED_RED_PIN, gpioModePushPull, 0);
    GPIO_PinModeSet(LED_GREEN_PORT, LED_GREEN_PIN, gpioModePushPull, 0);
    GPIO_PinModeSet(LED_BLUE_PORT, LED_BLUE_PIN, gpioModePushPull, 0);
}

/**
 * @brief Init TIMER0 to regulate PWM dutycycle.
 */
uint32_t timer0_init(void)
{
    // Enable clocks.
    CMU_ClockEnable(cmuClock_TIMER0, true);

    // TODO Init
    // Init CC for Output Compare on GPIO pins.
    TIMER_InitCC_TypeDef ccInit = TIMER_INITCC_DEFAULT;
    ccInit.mode = timerCCModeCompare;
    ccInit.cmoa = timerOutputActionToggle;

    // Initilize a CC channels for toggling leds.
    TIMER_InitCC(TIMER0, 0, &ccInit);

    // Enable GPIO toggling by TIMER and set location of led pins to be toggled.
    TIMER0->ROUTEPEN = TIMER_ROUTEPEN_CC0PEN;
    TIMER0->ROUTELOC0 = LED_RED_LOCATION | LED_GREEN_LOCATION | LED_BLUE_LOCATION;

    // Set same TIMER0 top value for all CC channels.
    TIMER_TopSet(TIMER0, TIMER0_TOP_VAL);

    // Timer general initialization and config.
    TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
    timerInit.prescale = TIMER0_CLOCK_DIV;
    timerInit.enable = true; // Start timer after init.

    TIMER_Init(TIMER0, &timerInit);

    // return calculated frequncy
    return (uint32_t)(CMU_ClockFreqGet(cmuClock_TIMER0) / (uint32_t)(0x1 << TIMER0_CLOCK_DIV));
}

// util function to set top value/duty cycle for timer0
void timer0_set_top_val(uint32_t tv)
{
    TIMER_TopBufSet(TIMER0, tv);
}
