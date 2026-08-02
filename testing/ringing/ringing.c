#include <limits.h>
#include "pico/stdlib.h"

#define MAX_RINGS 3 // INT_MAX

#define RING_FREQ_HZ 20

#define RING_PERIOD_MS (1000 / RING_FREQ_HZ)
#define RING_DELAY_MS (RING_PERIOD_MS / 2)

#define PIN_BELL_1 14
#define PIN_BELL_2 15
#define PIN_LED PICO_DEFAULT_LED_PIN

const int ring_pattern_ms[] = {400, 200, 400, 2000};
const size_t ring_pattern_size = sizeof(ring_pattern_ms) / sizeof(ring_pattern_ms[0]);

void ring(int time_ms)
{
    const int num_waves = time_ms / RING_PERIOD_MS;

    for (int i = 0; i < num_waves; i++) {
        gpio_put(PIN_LED, true);
        gpio_put(PIN_BELL_1, true);
        gpio_put(PIN_BELL_2, false);
        sleep_ms(RING_DELAY_MS);
        gpio_put(PIN_LED, false);
        gpio_put(PIN_BELL_1, false);
        gpio_put(PIN_BELL_2, true);
        sleep_ms(RING_DELAY_MS);
    }

    gpio_put(PIN_BELL_1, false);
    gpio_put(PIN_BELL_2, false);
}

int main()
{
    gpio_init(PIN_LED);
    gpio_init(PIN_BELL_1);
    gpio_init(PIN_BELL_2);

    gpio_set_dir(PIN_LED, GPIO_OUT);
    gpio_set_dir(PIN_BELL_1, GPIO_OUT);
    gpio_set_dir(PIN_BELL_2, GPIO_OUT);

    sleep_ms(1000);

    for (int i = 0; i < MAX_RINGS; i++) {
        for (int j = 0; j < ring_pattern_size; j++) {
            if (j & 1) {
                sleep_ms(ring_pattern_ms[j]);
            }
            else {
                ring(ring_pattern_ms[j]);
            }
        }
    }
}
