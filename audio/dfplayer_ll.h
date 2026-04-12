#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "dfplayer.h"

typedef enum
{
    DFP_CMD_NEXT = 0x01,                      // Next
    DFP_CMD_PREV = 0x02,                      // Previous
    DFP_CMD_PLAY_TRACK_ROOT = 0x03,           // Specify playback of a track
    DFP_CMD_INC_VOLUME = 0x04,                // Increase volume
    DFP_CMD_DEC_VOLUME = 0x05,                // Decrease volume
    DFP_CMD_SET_VOLUME = 0x06,                // Specify volume (0-30)
    DFP_CMD_SET_EQ = 0x07,                    // Specify EQ (0-5 = Normal/Pop/Rock/Jazz/Classic/Bass)
    DFP_CMD_SINGLE_REPEAT_PLAYBACK = 0x08,    // Specify single repeat playback
    DFP_CMD_SET_DEVICE = 0x09,                // Specify playback of a device (USB/SD)
    DFP_CMD_SLEEP = 0x0A,                     // Set sleep
    DFP_CMD_RESET = 0x0C,                     // Reset module
    DFP_CMD_PLAY = 0x0D,                      // Play
    DFP_CMD_PAUSE = 0x0E,                     // Pause
    DFP_CMD_PLAY_TRACK_FOLDER = 0x0F,         // Specify playback a track in a folder
    DFP_CMD_SET_AMP_GAIN = 0x10,              // Audio amplification setting (MSB=1: amplifying on, LSB:set gain 0-31)
    DFP_CMD_SET_REPEAT_ALL = 0x11,            // Set all repeat playback
    DFP_CMD_PLAY_TRACK_MP3_FOLDER = 0x12,     // Specify playback of folder named "MP3"
    DFP_CMD_INSERT_ADVERT = 0x13,             // Insert an advertisement
    DFP_CMD_PLAY_TRACK_LARGE_FOLDER = 0x14,   // Specify playback a track in a folder that supports 3000 tracks
    DFP_CMD_STOP_ADVERT = 0x15,               // Stop playing inserted advertisement and go back to play the music interrupted
    DFP_CMD_STOP = 0x16,                      // Stop
    DFP_CMD_PLAY_FOLDER_REPEAT = 0x17,        // Specify repeat playback of a folder
    DFP_CMD_RANDOM_PLAYBACK = 0x18,           // Set random playback
    DFP_CMD_REPEAT_CURRENT = 0x19,            // Set repeat playback of current track
    DFP_CMD_SET_DAC = 0x1A,                   // Set DAC
    DFP_CMD_QUERY_ONLINE_STORAGE = 0x3F,      // Query current online storage device
    DFP_CMD_QUERY_STATUS = 0x42,              // Query current status
    DFP_CMD_QUERY_VOLUME = 0x43,              // Query current volume
    DFP_CMD_QUERY_EQ = 0x44,                  // Query current EQ
    DFP_CMD_QUERY_NUM_TRACKS_USB_ROOT = 0x47, // Query number of tracks in the root of USB flash drive
    DFP_CMD_QUERY_NUM_TRACKS_SD_ROOT = 0x48,  // Query number of tracks in the root of micro SD card
    DFP_CMD_QUERY_CURRENT_TRACK_USB = 0x4B,   // Query current track in the USB flash drive
    DFP_CMD_QUERY_CURRENT_TRACK_SD = 0x4C,    // Query current track in the micro SD Card
    DFP_CMD_QUERY_NUM_TRACKS_FOLDER = 0x4E,   // Query number of tracks in a folder
    DFP_CMD_QUERY_NUM_FOLDERS = 0x4F,         // Query number of folders in the current storage device
} dfp_cmd_t;

typedef enum
{
    DFP_RECV_STORAGE_DEVICE_INSERTED = 0x3A, // Storage device was plugged in
    DFP_RECV_STORAGE_DEVICE_REMOVED = 0x3B,  // Storage device was removed
    DFP_RECV_PLAYBACK_FINISHED_USB = 0x3C,   // Track finished playing from USB
    DFP_RECV_PLAYBACK_FINISHED_SD = 0x3D,    // Track finished playing from SD
    DFP_RECV_POWER_ON = 0x3F,                // Module powered on
    DFP_RECV_ERROR = 0x40,                   // Error data
    DFP_RECV_FEEDBACK = 0x41,                // Normal feedback data
} dfp_recv_t;

typedef enum
{
    DFP_RECV_ERR_BUSY = 0x01,            // Module busy (initialisation not complete)
    DFP_RECV_ERR_SLEEPING = 0x02,        // Device currently in sleep mode
    DFP_RECV_ERR_SERIAL = 0x03,          // Serial receive error
    DFP_RECV_ERR_CHECKSUM = 0x04,        // Bad checksum
    DFP_RECV_ERR_TRACK_SCOPE = 0x05,     // Track out of scope
    DFP_RECV_ERR_TRACK_NOT_FOUND = 0x06, // Track not found
    DFP_RECV_ERR_INSERTION = 0x07,       // Insertion operation error
    DFP_RECV_ERR_SD = 0x08,              // SD card read failure
    DFP_RECV_ERR_ENTERED_SLEEP = 0x0A,   // Entered sleep mode
} dfp_recv_err_t;

dfp_err_t dfp_send_command(uart_inst_t *uart, dfp_cmd_t cmd, uint16_t param, bool feedback);
dfp_err_t dfp_recv_command(uart_inst_t *uart, dfp_cmd_t cmd, uint16_t *param);
dfp_err_t dfp_send_recv_command(uart_inst_t *uart, dfp_cmd_t cmd, uint16_t *param, bool feedback);
