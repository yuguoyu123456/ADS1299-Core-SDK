#include <assert.h>
#include <math.h>
#include <stdint.h>

#include "ads1299_lsl_bridge.h"
#include "ads1299_packet.h"

int main(void)
{
    uint8_t packet[ADS1299_PACKET_SIZE];
    uint8_t status[ADS1299_PACKET_STATUS_BYTES] = {0xC0u, 0x00u, 0x00u};
    int32_t channels[ADS1299_PACKET_CHANNELS] = {
        0, 1000, -1000, 2000, -2000, 3000, -3000, 4000
    };
    ads1299_lsl_sample_t sample;
    float copied[ADS1299_PACKET_CHANNELS];
    size_t i;

    assert(ads1299_packet_encode(packet,
                                 sizeof(packet),
                                 ADS1299_PACKET_FLAG_DATA_VALID |
                                     ADS1299_PACKET_FLAG_TIMESTAMP_VALID,
                                 42u,
                                 123456u,
                                 status,
                                 channels) == ADS1299_PACKET_SIZE);

    assert(ads1299_lsl_packet_to_sample(packet,
                                        sizeof(packet),
                                        4.5,
                                        24u,
                                        &sample) == 0);
    assert(sample.sequence == 42u);
    assert(sample.device_timestamp_us == 123456u);
    assert(sample.flags & ADS1299_PACKET_FLAG_DATA_VALID);
    assert(sample.status[0] == 0xC0u);

    ads1299_lsl_copy_channels(&sample, copied);
    for (i = 0u; i < ADS1299_PACKET_CHANNELS; ++i)
        assert(fabs((double)copied[i] - (double)sample.channels_uv[i]) < 0.0001);

    /* Corrupt canonical packet CRC and confirm bridge rejects it. */
    packet[20] ^= 0x01u;
    assert(ads1299_lsl_packet_to_sample(packet,
                                        sizeof(packet),
                                        4.5,
                                        24u,
                                        &sample) == -3);

    return 0;
}
