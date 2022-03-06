/**
 * @file timer_handler.h
 *
 * @author Johannes Ehala, ProLab.
 * @license MIT
 *
 * Copyright ProLab, TTÜ. 15 January 2022
 */

#ifndef TIMER_HANDLER_H_
#define TIMER_HANDLER_H_

#include "em_gpio.h" /// import GPIO

#define LED_BLUE_PORT gpioPortA // !< Port for blue led.

#define LED_BLUE_PIN 5                                // !< Pin for blue led.
#define TIMER0_CLOCK_DIV timerPrescale16              // Timer0 clock divider
#define TIMER0_TOP_VAL 255                            // Timer0 top value
#define LED_BLUE_LOCATION TIMER_ROUTELOC0_CC2LOC_LOC3 //      Location for blue led.
#define fade_step_size 5                              // Amount by which brightness is faded
#define brightness 255                                // Maximum brightness

// Public functions
void led_gpio_init();                            // Initialize GPIO and set up pins for leds
uint32_t timer0_init(void);                      // Init TIMER0 to regulate PWM dutycycle
void duty_cycle_set(uint32_t dutyCycleChannel2); // Set TIMER0 top value

#endif // TIMER_HANDLER_H_ */
