#ifndef ADS1299_TRANSPORT_H
#define ADS1299_TRANSPORT_H

#include <stddef.h>
#include <stdint.h>

#include "ads1299_packet.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ADS1299_TRANSPORT_UART = 0,
    ADS1299_TRANSPORT_USB,
    ADS1299_TRANSPORT_BLE,
    ADS1299_TRANSPORT_WIFI,
    ADS1299_TRANSPORT_ETHERNET,
    ADS1299_TRANSPORT_LSL_BRIDGE
} ads1299_transport_kind_t;

/* Return the number of bytes accepted. Returning zero reports back-pressure
 * or an I/O failure to the caller; the callback must never block in DRDY ISR
 * context because transport submission belongs in a lower-priority task. */
typedef size_t (*ads1299_transport_write_fn)(void *user,
                                             const uint8_t *data,
                                             size_t length);
typedef int (*ads1299_transport_flush_fn)(void *user);

typedef struct {
    void *user;
    ads1299_transport_kind_t kind;
    ads1299_transport_write_fn write;
    ads1299_transport_flush_fn flush;
    size_t max_chunk;
} ads1299_transport_t;

typedef enum {
    ADS1299_TRANSPORT_OK = 0,
    ADS1299_TRANSPORT_INVALID_ARGUMENT = -1,
    ADS1299_TRANSPORT_WRITE_FAILED = -2,
    ADS1299_TRANSPORT_FLUSH_FAILED = -3,
    ADS1299_TRANSPORT_ENCODE_FAILED = -4
} ads1299_transport_status_t;

ads1299_transport_status_t ads1299_transport_send(
    const ads1299_transport_t *transport,
    const uint8_t *data,
    size_t length);

ads1299_transport_status_t ads1299_transport_send_sample(
    const ads1299_transport_t *transport,
    uint8_t flags,
    uint32_t sequence,
    uint32_t timestamp_us,
    const uint8_t status[ADS1299_PACKET_STATUS_BYTES],
    const int32_t channels[ADS1299_PACKET_CHANNELS]);

#ifdef __cplusplus
}
#endif

#endif
