#include <stdio.h>
#include "pico/stdlib.h"

#define PIN_DIALLING 16
#define PIN_PULSE 17

int main()
{
    stdio_init_all();

    gpio_init(PIN_DIALLING);
    gpio_init(PIN_PULSE);
    gpio_set_dir(PIN_DIALLING, GPIO_IN);
    gpio_set_dir(PIN_PULSE, GPIO_IN);
    gpio_pull_up(PIN_DIALLING);
    gpio_pull_up(PIN_PULSE);

    int dialled_digit = -1;

    printf("\nStarted.\n");

    while (true) {
        // Wait for dialling signal to go low
        while (gpio_get(PIN_DIALLING)) {
            sleep_ms(100);
        }

        printf("\nDialling...\n");

        int pulses = 0;
        bool last_pulse_state = false;

        // Count rising edges of pulse signal, while dialling signal remains low
        while (!gpio_get(PIN_DIALLING)) {
            bool pulse_state = gpio_get(PIN_PULSE);

            if (pulse_state && !last_pulse_state) {
                // Rising edge
                pulses++;
            }

            last_pulse_state = pulse_state;
            sleep_ms(10);
        }

        // Figure out what number we dialled
        if (pulses == 0) {
            dialled_digit = -1;
            printf("Dialled digit: nothing");
        }
        else {
            if (pulses == 10) {
                dialled_digit = 0;
            }
            else {
                dialled_digit = pulses;
            }
            printf("Dialled digit: %d", dialled_digit);
        }
    }
}
