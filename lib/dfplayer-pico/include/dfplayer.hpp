#pragma once

extern "C"
{
#include <dfplayer.h>
}

struct DFPlayer
{
    explicit DFPlayer(uart_inst_t *uart)
    {
        _uart = uart;
    }

    inline dfp_err_t reset(bool feedback = true)
    {
        return dfp_reset(_uart, feedback);
    }

    inline dfp_err_t play(bool feedback = true)
    {
        return dfp_play(_uart, feedback);
    }

    inline dfp_err_t pause(bool feedback = true)
    {
        return dfp_pause(_uart, feedback);
    }

    inline dfp_err_t stop(bool feedback = true)
    {
        return dfp_stop(_uart, feedback);
    }

    inline dfp_err_t prev(bool feedback = true)
    {
        return dfp_prev(_uart, feedback);
    }

    inline dfp_err_t next(bool feedback = true)
    {
        return dfp_next(_uart, feedback);
    }

    inline dfp_err_t play_track_root(uint16_t track, bool feedback = true)
    {
        return dfp_play_track_root(_uart, feedback, track);
    }

    inline dfp_err_t play_track_mp3_folder(uint16_t track, bool feedback = true)
    {
        return dfp_play_track_mp3_folder(_uart, feedback, track);
    }

    inline dfp_err_t play_track_folder(uint8_t track, uint8_t folder, bool feedback = true)
    {
        return dfp_play_track_folder(_uart, feedback, track, folder);
    }

    inline dfp_err_t play_track_large_folder(uint16_t track, uint8_t folder, bool feedback = true)
    {
        return dfp_play_track_large_folder(_uart, feedback, track, folder);
    }

    inline dfp_err_t inc_volume(bool feedback = true)
    {
        return dfp_inc_volume(_uart, feedback);
    }

    inline dfp_err_t dec_volume(bool feedback = true)
    {
        return dfp_dec_volume(_uart, feedback);
    }

    inline dfp_err_t set_volume(uint8_t volume, bool feedback = true)
    {
        return dfp_set_volume(_uart, feedback, volume);
    }

    inline dfp_err_t query_volume(uint8_t *volume, bool feedback = true)
    {
        return dfp_query_volume(_uart, feedback, volume);
    }

    inline dfp_err_t query_status(dfp_playback_status_t *playback_status, dfp_device_status_t *device_status, bool feedback = true)
    {
        return dfp_query_status(_uart, feedback, playback_status, device_status);
    }

    inline dfp_err_t get_message(dfp_message_t *message, uint16_t *param, uint32_t timeout_ms)
    {
        return dfp_get_message(_uart, message, param, timeout_ms);
    }

private:
    uart_inst_t *_uart;
};
