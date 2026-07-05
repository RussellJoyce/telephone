#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "dfplayer_err.h"

#define DFP_BAUD_RATE 9600

#define DFP_MIN_VOLUME 0
#define DFP_MAX_VOLUME 30

#define DFP_MIN_TRACK_ROOT 1
#define DFP_MAX_TRACK_ROOT 3000

#define DFP_MIN_TRACK_MP3 1
#define DFP_MAX_TRACK_MP3 65535

#define DFP_MIN_TRACK_FOLDER 1
#define DFP_MAX_TRACK_FOLDER 255
#define DFP_MIN_NUM_FOLDER 1
#define DFP_MAX_NUM_FOLDER 99

#define DFP_MIN_TRACK_LARGE_FOLDER 1
#define DFP_MAX_TRACK_LARGE_FOLDER 3000
#define DFP_MIN_NUM_LARGE_FOLDER 1
#define DFP_MAX_NUM_LARGE_FOLDER 15

typedef enum
{
    DFP_PLAYBACK_STATUS_STOPPED = 0x00,
    DFP_PLAYBACK_STATUS_PLAYING = 0x01,
    DFP_PLAYBACK_STATUS_PAUSED = 0x02,
} dfp_playback_status_t;

typedef enum
{
    DFP_DEVICE_STATUS_USB = 0x01,
    DFP_DEVICE_STATUS_SD = 0x02,
    DFP_DEVICE_STATUS_SLEEPING = 0x10,
} dfp_device_status_t;

typedef enum
{
    DFP_MSG_NONE = 0x00,                    // No message received
    DFP_MSG_STORAGE_DEVICE_INSERTED = 0x3A, // Storage device was plugged in
    DFP_MSG_STORAGE_DEVICE_REMOVED = 0x3B,  // Storage device was removed
    DFP_MSG_PLAYBACK_FINISHED_USB = 0x3C,   // Track finished playing from USB
    DFP_MSG_PLAYBACK_FINISHED_SD = 0x3D,    // Track finished playing from SD
    DFP_MSG_POWER_ON = 0x3F,                // Module powered on
    DFP_MSG_ERROR = 0x40,                   // Error data
} dfp_message_t;

dfp_err_t dfp_reset(uart_inst_t *uart, bool feedback);
dfp_err_t dfp_play(uart_inst_t *uart, bool feedback);
dfp_err_t dfp_pause(uart_inst_t *uart, bool feedback);
dfp_err_t dfp_stop(uart_inst_t *uart, bool feedback);
dfp_err_t dfp_prev(uart_inst_t *uart, bool feedback);
dfp_err_t dfp_next(uart_inst_t *uart, bool feedback);

dfp_err_t dfp_play_track_root(uart_inst_t *uart, bool feedback, uint16_t track);
dfp_err_t dfp_play_track_mp3_folder(uart_inst_t *uart, bool feedback, uint16_t track);
dfp_err_t dfp_play_track_folder(uart_inst_t *uart, bool feedback, uint8_t track, uint8_t folder);
dfp_err_t dfp_play_track_large_folder(uart_inst_t *uart, bool feedback, uint16_t track, uint8_t folder);

dfp_err_t dfp_inc_volume(uart_inst_t *uart, bool feedback);
dfp_err_t dfp_dec_volume(uart_inst_t *uart, bool feedback);
dfp_err_t dfp_set_volume(uart_inst_t *uart, bool feedback, uint8_t volume);

dfp_err_t dfp_query_volume(uart_inst_t *uart, bool feedback, uint8_t *volume);
dfp_err_t dfp_query_status(uart_inst_t *uart, bool feedback, dfp_playback_status_t *playback_status, dfp_device_status_t *device_status);

dfp_err_t dfp_get_message(uart_inst_t *uart, dfp_message_t *message, uint16_t *param, uint32_t timeout_ms);
