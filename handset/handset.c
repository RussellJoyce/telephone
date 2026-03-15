#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/sleep.h"

/*
 * Connect PIN_HANDSET to T3.
 * Connect GND to T19.
 * These will be shorted (via ~32 ohms resistance) when handset is lifted.
*/

#define PIN_HANDSET 18
#define PIN_LED PICO_DEFAULT_LED_PIN

void sleep()
{
    // UART will be reconfigured by sleep_run_from_xosc
    uart_default_tx_wait_blocking();
    sleep_run_from_xosc();

    printf("Handset replaced. Sleeping now...\n");
    uart_default_tx_wait_blocking();

    sleep_goto_dormant_until_pin(PIN_HANDSET, true, false);
    sleep_power_up();

    printf("Handset lifted. Woken up!\n");
}

void init_gpio()
{
    gpio_init(PIN_LED);
    gpio_set_dir(PIN_LED, GPIO_OUT);

    gpio_init(PIN_HANDSET);
    gpio_set_dir(PIN_HANDSET, GPIO_IN);
    gpio_pull_up(PIN_HANDSET);
}

int main()
{
    stdio_init_all();

    init_gpio();
    gpio_put(PIN_LED, false);

    printf("\nStarted handset test.\n");

    while (true) {
        bool on_hook = gpio_get(PIN_HANDSET);
        gpio_put(PIN_LED, !on_hook);

        if (on_hook) {
            sleep();
            init_gpio();
        }

        sleep_ms(100);
    }
}
