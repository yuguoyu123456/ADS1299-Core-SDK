#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "ads1299_transport.h"

typedef struct {
    uint8_t bytes[ADS1299_PACKET_SIZE];
    size_t length;
    size_t accepted_per_call;
    unsigned calls;
    unsigned flushes;
    int flush_result;
} sink_t;

static size_t sink_write(void *user, const uint8_t *data, size_t length) {
    sink_t *sink = (sink_t *)user;
    size_t accepted = length;
    if (sink->accepted_per_call != 0u && accepted > sink->accepted_per_call) {
        accepted = sink->accepted_per_call;
    }
    assert(sink->length + accepted <= sizeof(sink->bytes));
    memcpy(&sink->bytes[sink->length], data, accepted);
    sink->length += accepted;
    sink->calls++;
    return accepted;
}

static size_t fail_write(void *user, const uint8_t *data, size_t length) {
    (void)user;
    (void)data;
    (void)length;
    return 0u;
}

static int sink_flush(void *user) {
    sink_t *sink = (sink_t *)user;
    sink->flushes++;
    return sink->flush_result;
}

int main(void) {
    const uint8_t status[ADS1299_PACKET_STATUS_BYTES] = {0xC0u, 0x00u, 0x00u};
    const int32_t channels[ADS1299_PACKET_CHANNELS] = {1, -1, 2, -2, 3, -3, 4, -4};
    sink_t sink = {.accepted_per_call = 3u};
    ads1299_transport_t transport = {
        .user = &sink,
        .kind = ADS1299_TRANSPORT_BLE,
        .write = sink_write,
        .flush = sink_flush,
        .max_chunk = 7u,
    };

    assert(ads1299_transport_send_sample(&transport, 0x12u, 42u, 1000u,
                                         status, channels) == ADS1299_TRANSPORT_OK);
    assert(sink.length == ADS1299_PACKET_SIZE);
    assert(sink.calls > 1u);
    assert(sink.flushes == 1u);
    assert(sink.bytes[0] == ADS1299_PACKET_SYNC0);
    assert(sink.bytes[1] == ADS1299_PACKET_SYNC1);

    transport.write = fail_write;
    assert(ads1299_transport_send(&transport, status, sizeof(status)) ==
           ADS1299_TRANSPORT_WRITE_FAILED);
    assert(ads1299_transport_send(NULL, status, sizeof(status)) ==
           ADS1299_TRANSPORT_INVALID_ARGUMENT);

    transport.write = sink_write;
    sink.flush_result = -1;
    assert(ads1299_transport_send(&transport, NULL, 0u) ==
           ADS1299_TRANSPORT_FLUSH_FAILED);
    return 0;
}
