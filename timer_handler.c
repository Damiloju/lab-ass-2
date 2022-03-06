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
    ccInit.mode = timerCCModePWM;
    ccInit.cofoa = timerOutputActionToggle;

    // Initilize a CC channels for toggling leds.
    TIMER_InitCC(TIMER0, 0, &ccInit); // channel 0
    TIMER_InitCC(TIMER0, 1, &ccInit); // channel 1
    TIMER_InitCC(TIMER0, 2, &ccInit); // channel 2

    // Set same TIMER0 top value for all CC channels.
    TIMER_TopSet(TIMER0, TIMER0_TOP_VAL);
    TIMER_CompareSet(TIMER0, 0, TIMER0_TOP_VAL);      // Set compare starting value for channel 0
    TIMER_CompareSet(TIMER0, 1, TIMER0_TOP_VAL - 10); // Set compare starting value value for channel 1
    TIMER_CompareSet(TIMER0, 2, TIMER0_TOP_VAL - 20); // Set compare starting value value for channel 2

    // Channel 0 Enable GPIO toggling by TIMER and set location of led pins to be toggled.
    TIMER0->ROUTEPEN = TIMER_ROUTEPEN_CC0PEN;
    TIMER0->ROUTELOC0 = LED_GREEN_LOCATION;

    // Channel 1 Enable GPIO toggling by TIMER and set location of led pins to be toggled.
    TIMER0->ROUTEPEN = TIMER_ROUTEPEN_CC1PEN;
    TIMER0->ROUTELOC0 = LED_RED_LOCATION;

    // Channel 2 Enable GPIO toggling by TIMER and set location of led pins to be toggled.
    TIMER0->ROUTEPEN = TIMER_ROUTEPEN_CC2PEN;
    TIMER0->ROUTELOC0 = LED_BLUE_LOCATION;

    // Timer general initialization and config.
    TIMER_Init_TypeDef timerInit = TIMER_INIT_DEFAULT;
    timerInit.prescale = TIMER0_CLOCK_DIV;
    timerInit.enable = true; // Start timer after init.

    TIMER_Init(TIMER0, &timerInit);

    // return calculated frequncy
    return (uint32_t)(CMU_ClockFreqGet(cmuClock_TIMER0) / (uint32_t)(0x1 << TIMER0_CLOCK_DIV));
}

// util function to set top value/duty cycle for timer0
void duty_cycle_set(uint32_t tv)
{
    TIMER_CompareBufSet(TIMER0, 0, tv);
}
