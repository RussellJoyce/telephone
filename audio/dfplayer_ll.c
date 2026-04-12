#include <stdint.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "dfplayer.h"
#include "dfplayer_ll.h"

#define DFP_PACKET_SIZE 10

static inline uint16_t calc_checksum(const uint8_t data[DFP_PACKET_SIZE])
{
    uint16_t sum = 0;
    for (int i = 1; i < 7; i++)
    {
        sum += data[i];
    }
    return 0xffff - sum + 1;
}

static inline void fill_checksum(uint8_t data[DFP_PACKET_SIZE])
{
    uint16_t sum = calc_checksum(data);
    data[7] = sum >> 8;
    data[8] = sum & 0xFF;
}

static inline bool validate_checksum(const uint8_t data[DFP_PACKET_SIZE])
{
    uint16_t sum = calc_checksum(data);
    return (data[7] == (sum >> 8)) && (data[8] == (sum & 0xFF));
}

static inline dfp_err_t lookup_recv_error(dfp_recv_err_t recv_error)
{
    switch (recv_error)
    {
    case DFP_RECV_ERR_BUSY:
        return DFP_ERR_BUSY;
    case DFP_RECV_ERR_SLEEPING:
        return DFP_ERR_SLEEPING;
    case DFP_RECV_ERR_SERIAL:
        return DFP_ERR_SERIAL;
    case DFP_RECV_ERR_CHECKSUM:
        return DFP_ERR_SENT_CHECKSUM_BAD;
    case DFP_RECV_ERR_TRACK_SCOPE:
        return DFP_ERR_TRACK_OUT_OF_SCOPE;
    case DFP_RECV_ERR_TRACK_NOT_FOUND:
        return DFP_ERR_TRACK_NOT_FOUND;
    case DFP_RECV_ERR_INSERTION:
        return DFP_ERR_INSERTION_FAILED;
    case DFP_RECV_ERR_SD:
        return DFP_ERR_SD_READ_FAILED;
    case DFP_RECV_ERR_ENTERED_SLEEP:
        return DFP_ERR_OK;
    default:
        return DFP_ERR_UNKNOWN_ERROR;
    }
}

dfp_err_t dfp_send_command(uart_inst_t *uart, dfp_cmd_t cmd, uint16_t param, bool feedback)
{
    uint8_t data[DFP_PACKET_SIZE] = {
        0x7E,                   // start byte
        0xFF,                   // version
        0x06,                   // length
        cmd,                    // command
        feedback ? 0x01 : 0x00, // feedback
        param >> 8,             // param high byte
        param & 0xFF,           // param low byte
        0x00,                   // checksum high byte
        0x00,                   // checksum low byte
        0xEF,                   // end byte
    };

    fill_checksum(data);

    uart_write_blocking(uart, data, DFP_PACKET_SIZE);

    if (!feedback)
    {
        return DFP_ERR_OK;
    }

    uart_read_blocking(uart, data, DFP_PACKET_SIZE);

    if (!validate_checksum(data))
    {
        return DFP_ERR_RECEIVED_CHECKSUM_BAD;
    }

    dfp_recv_t recv_type = data[3];
    switch (recv_type)
    {
    case DFP_RECV_FEEDBACK:
        return DFP_ERR_OK;
    case DFP_RECV_ERROR:
    {
        dfp_recv_err_t recv_err = data[6];
        return lookup_recv_error(recv_err);
    }
    case DFP_RECV_STORAGE_DEVICE_INSERTED:
    case DFP_RECV_STORAGE_DEVICE_REMOVED:
    case DFP_RECV_PLAYBACK_FINISHED_USB:
    case DFP_RECV_PLAYBACK_FINISHED_SD:
    case DFP_RECV_POWER_ON:
        // TODO: Handle this properly
        return DFP_ERR_UNEXPECTED_DATA;
    default:
        return DFP_ERR_UNKNOWN_DATA;
    }
}

dfp_err_t dfp_recv_command(uart_inst_t *uart, dfp_cmd_t cmd, uint16_t *param)
{
    uint8_t data[DFP_PACKET_SIZE];

    uart_read_blocking(uart, data, DFP_PACKET_SIZE);

    if (!validate_checksum(data))
    {
        return DFP_ERR_RECEIVED_CHECKSUM_BAD;
    }

    dfp_recv_t recv_param_h = data[5];
    dfp_recv_t recv_param_l = data[6];
    *param = (recv_param_h << 8) | recv_param_l;

    dfp_recv_t recv_cmd = data[3];
    if (recv_cmd != cmd)
    {
        // TODO: Handle this properly
        return DFP_ERR_UNEXPECTED_DATA;
    }

    return DFP_ERR_OK;
}

dfp_err_t dfp_send_recv_command(uart_inst_t *uart, dfp_cmd_t cmd, uint16_t *param, bool feedback)
{
    dfp_err_t err = dfp_send_command(uart, cmd, 0, feedback);
    if (err != DFP_ERR_OK)
    {
        return err;
    }

    return dfp_recv_command(uart, cmd, param);
}
