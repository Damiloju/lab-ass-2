/**
 * @brief Example usage of Timer peripheral.
 *
 * EFR32 Application Note on Timers
 * https://www.silabs.com/documents/public/application-notes/AN0014.pdf
 *
 * EFR32MG12 Wireless Gecko Reference Manual (Timer p672)
 * https://www.silabs.com/documents/public/reference-manuals/efr32xg12-rm.pdf
 *
 * Timer API documentation
 * https://docs.silabs.com/mcu/latest/efr32mg12/group-TIMER
 *
 * ARM RTOS API
 * https://arm-software.github.io/CMSIS_5/RTOS2/html/group__CMSIS__RTOS.html
 *
 * Copyright Thinnect Inc. 2019
 * Copyright ProLab TTÜ 2022
 * @license MIT
 * @author Johannes Ehala
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>

#include "retargetserial.h"
#include "cmsis_os2.h"
#include "platform.h"

#include "SignatureArea.h"
#include "DeviceSignature.h"

#include "loggers_ext.h"
#include "logger_fwrite.h"

#include "timer_handler.h"

#include "loglevels.h"
#define __MODUUL__ "main"
#define __LOG_LEVEL__ (LOG_LEVEL_main & BASE_LOG_LEVEL)
#include "log.h"

// Include the information header binary
#include "incbin.h"
INCBIN(Header, "header.bin");

static uint32_t timer_freq;
void led_control_loop(void *args);
uint32_t fade_step_size 5; // Amount by which brightness is faded
uint32_t brightness 255;   // Maximum brightness
// Heartbeat thread, initialize Timer and print heartbeat messages.
void hp_loop()
{
#define ESWGPIO_HB_DELAY 10 // Heartbeat message delay, seconds

    // Initialize GPIO and set up pins for leds
    led_gpio_init();

    // TODO Initialize Timer.
    timer_freq = timer0_init();

    info1("Timer frequency %lu Hz", timer_freq);

    // Create a thread for buzzer control.
    const osThreadAttr_t led_thread_attr = {.name = "buz_onoff"};
    osThreadNew(led_control_loop, NULL, &led_thread_attr);

    for (;;)
    {
        osDelay(ESWGPIO_HB_DELAY * osKernelGetTickFreq());
        info1("Heartbeat");
    }
}

void led_control_loop(void *args)
{

    for (;;)
    {
        // Gradually increase or reduce the brightness of the LED.
        brightness = brightness + fade_step_size;
        duty_cycle_set(brightness);

        // switch the logic of the face amount based on if we are at the max of min
        // fade amount logic was dubbed from the shared video
        if (brightness <= 0 || brightness >= 255)
        {
            fade_step_size = -fade_step_size;
        }

        // delay for 20 os ticks
        osDelay(20);
    }
}

int logger_fwrite_boot(const char *ptr, int len)
{
    fwrite(ptr, len, 1, stdout);
    fflush(stdout);
    return len;
}

int main()
{
    PLATFORM_Init();

    // Configure log message output
    RETARGET_SerialInit();
    log_init(BASE_LOG_LEVEL, &logger_fwrite_boot, NULL);

    info1("ESW-Timer " VERSION_STR " (%d.%d.%d)", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);

    // Initialize OS kernel.
    osKernelInitialize();

    // Create a thread.
    const osThreadAttr_t hp_thread_attr = {.name = "hp"};
    osThreadNew(hp_loop, NULL, &hp_thread_attr);

    if (osKernelReady == osKernelGetState())
    {
        // Switch to a thread-safe logger
        logger_fwrite_init();
        log_init(BASE_LOG_LEVEL, &logger_fwrite, NULL);

        // Start the kernel
        osKernelStart();
    }
    else
    {
        err1("!osKernelReady");
    }

    for (;;)
        ;
}
