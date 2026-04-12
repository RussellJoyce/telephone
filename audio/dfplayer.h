#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"

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
    DFP_ERR_OK = 0x00,
    DFP_ERR_BAD_ARGUMENT,
    DFP_ERR_BUSY,
    DFP_ERR_SLEEPING,
    DFP_ERR_SERIAL,
    DFP_ERR_SENT_CHECKSUM_BAD,
    DFP_ERR_RECEIVED_CHECKSUM_BAD,
    DFP_ERR_TRACK_OUT_OF_SCOPE,
    DFP_ERR_TRACK_NOT_FOUND,
    DFP_ERR_INSERTION_FAILED,
    DFP_ERR_SD_READ_FAILED,
    DFP_ERR_UNEXPECTED_DATA,
    DFP_ERR_UNKNOWN_DATA,
    DFP_ERR_UNKNOWN_ERROR,
} dfp_err_t;

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
