#include <stdint.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "dfplayer_ll.h"

#define DFP_PACKET_SIZE 10
#define DFP_DEFAULT_UART_TIMEOUT_MS 500

#define DFP_MSG_START_BYTE 0x7E
#define DFP_MSG_VERSION 0xFF
#define DFP_MSG_LENGTH 0x06
#define DFP_MSG_END_BYTE 0xEF

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

static dfp_err_t validate_message(const uint8_t data[DFP_PACKET_SIZE])
{
    if (data[0] != DFP_MSG_START_BYTE)
    {
        return DFP_ERR_INVALID_MSG_START_BYTE;
    }
    if (data[1] != DFP_MSG_VERSION)
    {
        return DFP_ERR_INVALID_MSG_VERSION;
    }
    if (data[2] != DFP_MSG_LENGTH)
    {
        return DFP_ERR_INVALID_MSG_LENGTH;
    }
    if (data[9] != DFP_MSG_END_BYTE)
    {
        return DFP_ERR_INVALID_MSG_END_BYTE;
    }
    if (!validate_checksum(data))
    {
        return DFP_ERR_INVALID_MSG_CHECKSUM;
    }

    return DFP_ERR_OK;
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

static inline size_t uart_read_blocking_timeout(uart_inst_t *uart, uint8_t *dst, size_t len, uint32_t timeout_ms)
{
    absolute_time_t timeout_time = make_timeout_time_ms(timeout_ms);
    size_t count;
    for (count = 0; count < len; count++)
    {
        while (!uart_is_readable(uart))
        {
            if (time_reached(timeout_time))
            {
                return count;
            }
        }
        *dst++ = (uint8_t)uart_get_hw(uart)->dr;
    }
    return count;
}

static inline bool uart_flush_until_byte(uart_inst_t *uart, uint8_t byte)
{
    while (uart_is_readable(uart))
    {
        if (uart_getc(uart) == byte)
        {
            return true;
        }
    }
    return false;
}

static inline bool uart_flush_until_start_byte(uart_inst_t *uart)
{
    return uart_flush_until_byte(uart, DFP_MSG_START_BYTE);
}

static inline bool uart_flush_until_end_byte(uart_inst_t *uart)
{
    return uart_flush_until_byte(uart, DFP_MSG_END_BYTE);
}

dfp_err_t dfp_send_command(uart_inst_t *uart, dfp_cmd_t cmd, uint16_t param, bool feedback)
{
    uint8_t data[DFP_PACKET_SIZE] = {
        DFP_MSG_START_BYTE,     // start byte
        DFP_MSG_VERSION,        // version
        DFP_MSG_LENGTH,         // length
        cmd,                    // command
        feedback ? 0x01 : 0x00, // feedback
        param >> 8,             // param high byte
        param & 0xFF,           // param low byte
        0x00,                   // checksum high byte
        0x00,                   // checksum low byte
        DFP_MSG_END_BYTE,       // end byte
    };

    fill_checksum(data);

    uart_write_blocking(uart, data, DFP_PACKET_SIZE);

    if (!feedback)
    {
        return DFP_ERR_OK;
    }

    size_t read_count = uart_read_blocking_timeout(uart, data, DFP_PACKET_SIZE, DFP_DEFAULT_UART_TIMEOUT_MS);
    if (read_count < DFP_PACKET_SIZE)
    {
        return DFP_ERR_READ_TIMEOUT;
    }

    dfp_err_t recv_err = validate_message(data);
    if (recv_err == DFP_ERR_INVALID_MSG_CHECKSUM)
    {
        return DFP_ERR_RECEIVED_CHECKSUM_BAD;
    }
    if (recv_err != DFP_ERR_OK)
    {
        return recv_err;
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

    size_t read_count = uart_read_blocking_timeout(uart, data, DFP_PACKET_SIZE, DFP_DEFAULT_UART_TIMEOUT_MS);
    if (read_count < DFP_PACKET_SIZE)
    {
        return DFP_ERR_READ_TIMEOUT;
    }

    dfp_err_t recv_err = validate_message(data);
    if (recv_err == DFP_ERR_INVALID_MSG_CHECKSUM)
    {
        return DFP_ERR_RECEIVED_CHECKSUM_BAD;
    }
    if (recv_err != DFP_ERR_OK)
    {
        return recv_err;
    }

    uint8_t recv_param_h = data[5];
    uint8_t recv_param_l = data[6];
    *param = (recv_param_h << 8) | recv_param_l;

    dfp_cmd_t recv_cmd = data[3];
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

dfp_err_t dfp_recv_async_status(uart_inst_t *uart, dfp_recv_t *status, uint16_t *param, uint32_t timeout_ms)
{
    uint8_t data[DFP_PACKET_SIZE];

    bool seen_start_byte = uart_flush_until_start_byte(uart);

    uint8_t *data_start = data;
    size_t data_size = DFP_PACKET_SIZE;

    if (seen_start_byte)
    {
        data[0] = DFP_MSG_START_BYTE;
        data_start++;
        data_size--;
    }

    size_t read_count = uart_read_blocking_timeout(uart, data_start, data_size, timeout_ms);
    if (read_count < data_size)
    {
        return DFP_ERR_READ_TIMEOUT;
    }

    dfp_err_t recv_err = validate_message(data);
    if (recv_err == DFP_ERR_INVALID_MSG_CHECKSUM)
    {
        return DFP_ERR_RECEIVED_CHECKSUM_BAD;
    }
    if (recv_err != DFP_ERR_OK)
    {
        return recv_err;
    }

    if (status != NULL)
    {
        *status = data[3];
    }
    if (param != NULL)
    {
        uint8_t recv_param_h = data[5];
        uint8_t recv_param_l = data[6];
        *param = (recv_param_h << 8) | recv_param_l;
    }

    // TODO: Check status message is valid?

    return DFP_ERR_OK;
}
