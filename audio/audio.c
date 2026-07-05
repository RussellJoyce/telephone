#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "dfplayer.h"

#define DFPLAYER_UART_ID uart1
#define DFPLAYER_UART_TX_PIN 4
#define DFPLAYER_UART_RX_PIN 5

#define PIN_LED PICO_DEFAULT_LED_PIN

void check_error(dfp_err_t err)
{
    if (err == DFP_ERR_OK)
    {
        printf("Ok\n");
    }
    else
    {
        printf("Error: %d %s\n", err, dfp_err_string(err));

        // Sleep forever
        sleep_ms(UINT32_MAX);
    }
}

int main()
{
    stdio_init_all();

    // Set up UART
    uart_init(DFPLAYER_UART_ID, DFP_BAUD_RATE);
    gpio_set_function(DFPLAYER_UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(DFPLAYER_UART_RX_PIN, GPIO_FUNC_UART);

    gpio_init(PIN_LED);
    gpio_set_dir(PIN_LED, GPIO_OUT);
    gpio_put(PIN_LED, false);

    printf("\n\n-------------------------------\n");
    printf("Started audio test.\n");

    dfp_err_t err;

    sleep_ms(1000);

    // Clear UART buffer and read any pending status messages (e.g. power on)
    printf("\nChecking for device messages...\n");
    dfp_message_t message = DFP_MSG_NONE;
    do
    {
        uint16_t param;
        err = dfp_get_message(DFPLAYER_UART_ID, &message, &param, 200);
        check_error(err);
        printf("Received message: 0x%hhx (%hu)\n", message, param);
    } while (message != DFP_MSG_NONE);

    printf("\nResetting module...\n");
    err = dfp_reset(DFPLAYER_UART_ID, true);
    check_error(err);

    sleep_ms(1000);

    // Clear UART buffer and read any pending status messages (e.g. power on)
    printf("\nChecking for device messages...\n");
    do
    {
        uint16_t param;
        err = dfp_get_message(DFPLAYER_UART_ID, &message, &param, 200);
        check_error(err);
        printf("Received message: 0x%hhx (%hu)\n", message, param);
    } while (message != DFP_MSG_NONE);

    printf("\nSetting volume...\n");
    err = dfp_set_volume(DFPLAYER_UART_ID, true, 10);
    check_error(err);

    // sleep_ms(1000);

    printf("\nPlaying root track 0001...\n");
    err = dfp_play_track_root(DFPLAYER_UART_ID, true, 1);
    check_error(err);

    sleep_ms(5000);

    printf("\nPlaying root track 0002...\n");
    err = dfp_play_track_root(DFPLAYER_UART_ID, true, 2);
    check_error(err);

    while (true)
    {
        sleep_ms(500);
        dfp_message_t message;
        uint16_t param;
        err = dfp_get_message(DFPLAYER_UART_ID, &message, &param, 500);
        check_error(err);
        printf("Received message: 0x%hhx (%hu)\n", message, param);
    }

    // sleep_ms(5000);

    // printf("\nStopping playback...\n");
    // err = dfp_stop(DFPLAYER_UART_ID, true);
    // check_error(err);
}
