#ifndef ADS1299_USB_STREAM_H
#define ADS1299_USB_STREAM_H

#include <stddef.h>
#include <stdint.h>

#include "ads1299_packet.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ADS1299_USB_BATCH_MAX_PACKETS 16u
#define ADS1299_USB_BATCH_MAX_BYTES \
    (ADS1299_USB_BATCH_MAX_PACKETS * ADS1299_PACKET_SIZE)

typedef struct {
    uint8_t buffer[ADS1299_PACKET_SIZE];
    size_t length;
    uint32_t packets_ok;
    uint32_t packets_bad;
    uint32_t bytes_discarded;
} ads1299_usb_stream_decoder_t;

/* Concatenate 1..16 already-encoded canonical packets for one USB TX request.
 * USB transfer boundaries are not part of the application protocol.
 * Returns bytes written, or 0 on error.
 */
size_t ads1299_usb_batch_encode(uint8_t *out,
                                size_t out_capacity,
                                const uint8_t *packets,
                                uint8_t packet_count);

void ads1299_usb_stream_decoder_init(ads1299_usb_stream_decoder_t *state);

/* Feed arbitrary USB/CDC/Bulk stream bytes one at a time.
 *  1: one complete validated canonical packet copied to packet_out
 *  0: no complete packet yet
 * <0: invalid argument
 *
 * CRC-invalid 49-byte candidates are internally discarded/resynchronized and
 * reported through counters rather than as fatal stream errors.
 */
int ads1299_usb_stream_decoder_push(ads1299_usb_stream_decoder_t *state,
                                    uint8_t byte,
                                    uint8_t packet_out[ADS1299_PACKET_SIZE]);

#ifdef __cplusplus
}
#endif

#endif
