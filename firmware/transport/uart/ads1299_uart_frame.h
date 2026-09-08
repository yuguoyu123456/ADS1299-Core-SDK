#ifndef ADS1299_UART_FRAME_H
#define ADS1299_UART_FRAME_H

#include <stddef.h>
#include <stdint.h>

#include "ads1299_packet.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ADS1299_UART_DELIMITER 0x00u
#define ADS1299_UART_COBS_MAX_ENCODED (ADS1299_PACKET_SIZE + 1u)
#define ADS1299_UART_MAX_WIRE_SIZE (ADS1299_UART_COBS_MAX_ENCODED + 1u)

typedef struct {
    uint8_t encoded[ADS1299_UART_COBS_MAX_ENCODED];
    size_t length;
    uint8_t dropping;
    uint32_t frames_ok;
    uint32_t frames_bad;
    uint32_t overflows;
} ads1299_uart_decoder_t;

/* Encode one validated canonical 49-byte ADS1299 packet using COBS and append
 * one zero delimiter. Returns bytes written, or 0 on error.
 */
size_t ads1299_uart_encode_packet(const uint8_t *packet,
                                  size_t packet_length,
                                  uint8_t *out,
                                  size_t out_capacity);

void ads1299_uart_decoder_init(ads1299_uart_decoder_t *state);

/* Feed one UART byte at a time.
 *  1: one complete validated ADS1299 packet copied to packet_out
 *  0: more bytes required or empty delimiter ignored
 * <0: malformed/overflowed frame discarded
 */
int ads1299_uart_decoder_push(ads1299_uart_decoder_t *state,
                              uint8_t byte,
                              uint8_t packet_out[ADS1299_PACKET_SIZE]);

#ifdef __cplusplus
}
#endif

#endif
