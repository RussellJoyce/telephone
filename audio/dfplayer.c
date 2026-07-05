#include <stdint.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "dfplayer.h"
#include "dfplayer_ll.h"

dfp_err_t dfp_reset(uart_inst_t *uart, bool feedback)
{
    return dfp_send_command(uart, DFP_CMD_RESET, 0, feedback);
}

dfp_err_t dfp_play(uart_inst_t *uart, bool feedback)
{
    return dfp_send_command(uart, DFP_CMD_PLAY, 0, feedback);
}

dfp_err_t dfp_pause(uart_inst_t *uart, bool feedback)
{
    return dfp_send_command(uart, DFP_CMD_PAUSE, 0, feedback);
}

dfp_err_t dfp_stop(uart_inst_t *uart, bool feedback)
{
    return dfp_send_command(uart, DFP_CMD_STOP, 0, feedback);
}

dfp_err_t dfp_prev(uart_inst_t *uart, bool feedback)
{
    return dfp_send_command(uart, DFP_CMD_PREV, 0, feedback);
}

dfp_err_t dfp_next(uart_inst_t *uart, bool feedback)
{
    return dfp_send_command(uart, DFP_CMD_NEXT, 0, feedback);
}

dfp_err_t dfp_play_track_root(uart_inst_t *uart, bool feedback, uint16_t track)
{
    if (track < DFP_MIN_TRACK_ROOT || track > DFP_MAX_TRACK_ROOT)
    {
        return DFP_ERR_BAD_ARGUMENT;
    }
    return dfp_send_command(uart, DFP_CMD_PLAY_TRACK_ROOT, track, feedback);
}

dfp_err_t dfp_play_track_mp3_folder(uart_inst_t *uart, bool feedback, uint16_t track)
{
    if (track < DFP_MIN_TRACK_MP3 || track > DFP_MAX_TRACK_MP3)
    {
        return DFP_ERR_BAD_ARGUMENT;
    }
    return dfp_send_command(uart, DFP_CMD_PLAY_TRACK_MP3_FOLDER, track, feedback);
}

dfp_err_t dfp_play_track_folder(uart_inst_t *uart, bool feedback, uint8_t track, uint8_t folder)
{
    if (track < DFP_MIN_TRACK_FOLDER || track > DFP_MAX_TRACK_FOLDER ||
        folder < DFP_MIN_NUM_FOLDER || folder > DFP_MAX_NUM_FOLDER)
    {
        return DFP_ERR_BAD_ARGUMENT;
    }
    uint16_t param = (folder << 8) | track;
    return dfp_send_command(uart, DFP_CMD_PLAY_TRACK_FOLDER, param, feedback);
}

dfp_err_t dfp_play_track_large_folder(uart_inst_t *uart, bool feedback, uint16_t track, uint8_t folder)
{
    if (track < DFP_MIN_TRACK_LARGE_FOLDER || track > DFP_MAX_TRACK_LARGE_FOLDER ||
        folder < DFP_MIN_NUM_LARGE_FOLDER || folder > DFP_MAX_NUM_LARGE_FOLDER)
    {
        return DFP_ERR_BAD_ARGUMENT;
    }
    uint16_t param = (folder << 12) | track & 0x0FFF;
    return dfp_send_command(uart, DFP_CMD_PLAY_TRACK_LARGE_FOLDER, param, feedback);
}

dfp_err_t dfp_inc_volume(uart_inst_t *uart, bool feedback)
{
    return dfp_send_command(uart, DFP_CMD_INC_VOLUME, 0, feedback);
}

dfp_err_t dfp_dec_volume(uart_inst_t *uart, bool feedback)
{
    return dfp_send_command(uart, DFP_CMD_DEC_VOLUME, 0, feedback);
}

dfp_err_t dfp_set_volume(uart_inst_t *uart, bool feedback, uint8_t volume)
{
    if (volume > DFP_MAX_VOLUME)
    {
        return DFP_ERR_BAD_ARGUMENT;
    }
    return dfp_send_command(uart, DFP_CMD_SET_VOLUME, volume, feedback);
}

dfp_err_t dfp_query_volume(uart_inst_t *uart, bool feedback, uint8_t *volume)
{
    uint16_t recv_param = 0;
    dfp_err_t err = dfp_send_recv_command(uart, DFP_CMD_QUERY_VOLUME, &recv_param, feedback);
    *volume = (uint8_t)recv_param;
    return err;
}

dfp_err_t dfp_query_status(uart_inst_t *uart, bool feedback, dfp_playback_status_t *playback_status, dfp_device_status_t *device_status)
{
    uint16_t recv_param = 0;
    dfp_err_t err = dfp_send_recv_command(uart, DFP_CMD_QUERY_STATUS, &recv_param, feedback);
    *playback_status = recv_param & 0xFF;
    *device_status = recv_param >> 8;
    return err;
}

dfp_err_t dfp_get_message(uart_inst_t *uart, dfp_message_t *message, uint16_t *param, uint32_t timeout_ms)
{
    dfp_recv_t status;
    dfp_err_t err = dfp_recv_async_status(uart, &status, param, timeout_ms);

    // Timeout means no message was received
    if (err == DFP_ERR_READ_TIMEOUT)
    {
        if (message != NULL)
        {
            *message = DFP_MSG_NONE;
        }
        if (param != NULL)
        {
            *param = 0;
        }
        return DFP_ERR_OK;
    }

    if (message != NULL)
    {
        *message = status;
    }
    return err;
}
