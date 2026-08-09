#include <cstdio>
#include <cstdint>
#include <cstdbool>
#include <print>
#include <utility>

#include "pico/stdlib.h"
#include "hardware/uart.h"

#include "dfplayer.hpp"

#define DFPLAYER_UART_ID uart1
#define DFPLAYER_UART_TX_PIN 4
#define DFPLAYER_UART_RX_PIN 5

#define PIN_LED PICO_DEFAULT_LED_PIN

using std::print;
using std::println;

void check_error(dfp_err_t err)
{
    if (err == DFP_ERR_OK)
    {
        println("Ok");
    }
    else
    {
        println("Error: {} {}", std::to_underlying(err), dfp_err_string(err));

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

    println("\n\n-------------------------------");
    println("Started audio test in C++");

    dfp_err_t err;

    DFPlayer player(DFPLAYER_UART_ID);

    sleep_ms(1000);

    // Clear UART buffer and read any pending status messages (e.g. power on)
    println("\nChecking for device messages...");
    dfp_message_t message = DFP_MSG_NONE;
    do
    {
        uint16_t param;
        err = player.get_message(&message, &param, 200);
        check_error(err);
        println("Received message: {:#x} ({})", std::to_underlying(message), param);
    } while (message != DFP_MSG_NONE);

    println("\nResetting module...");
    err = player.reset();
    check_error(err);

    sleep_ms(1000);

    // Clear UART buffer and read any pending status messages (e.g. power on)
    println("\nChecking for device messages...");
    do
    {
        uint16_t param;
        err = player.get_message(&message, &param, 200);
        check_error(err);
        println("Received message: {:#x} ({})", std::to_underlying(message), param);
    } while (message != DFP_MSG_NONE);

    println("\nSetting volume...");
    err = player.set_volume(10);
    check_error(err);

    // sleep_ms(1000);

    println("\nPlaying root track 0001...");
    err = player.play_track_root(1);
    check_error(err);

    sleep_ms(5000);

    println("\nPlaying root track 0002...");
    err = player.play_track_root(2);
    check_error(err);

    while (true)
    {
        sleep_ms(500);
        dfp_message_t message;
        uint16_t param;
        err = player.get_message(&message, &param, 500);
        check_error(err);
        println("Received message: {:#x} ({})", std::to_underlying(message), param);
    }

    // sleep_ms(5000);

    // println("\nStopping playback...");
    // err = player.stop();
    // check_error(err);
}
