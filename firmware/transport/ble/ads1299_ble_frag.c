#include "ads1299_ble_frag.h"

#include <string.h>

static void put_u32le(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)v;
    p[1] = (uint8_t)(v >> 8);
    p[2] = (uint8_t)(v >> 16);
    p[3] = (uint8_t)(v >> 24);
}

static uint32_t get_u32le(const uint8_t *p)
{
    return ((uint32_t)p[0]) |
           ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

size_t ads1299_ble_fragment_count(size_t packet_length, size_t att_payload_max)
{
    size_t payload_per_fragment;
    size_t count;

    if (packet_length == 0u || att_payload_max <= ADS1299_BLE_FRAG_HEADER_SIZE)
        return 0u;

    payload_per_fragment = att_payload_max - ADS1299_BLE_FRAG_HEADER_SIZE;
    count = (packet_length + payload_per_fragment - 1u) / payload_per_fragment;
    return count <= ADS1299_BLE_MAX_FRAGMENTS ? count : 0u;
}

size_t ads1299_ble_make_fragment(const uint8_t *packet,
                                 size_t packet_length,
                                 uint32_t packet_sequence,
                                 size_t att_payload_max,
                                 uint8_t fragment_index,
                                 uint8_t *out,
                                 size_t out_capacity)
{
    size_t count;
    size_t payload_per_fragment;
    size_t offset;
    size_t remaining;
    size_t payload_length;
    size_t total;

    if (!packet || !out) return 0u;
    count = ads1299_ble_fragment_count(packet_length, att_payload_max);
    if (count == 0u || fragment_index >= count) return 0u;

    payload_per_fragment = att_payload_max - ADS1299_BLE_FRAG_HEADER_SIZE;
    offset = (size_t)fragment_index * payload_per_fragment;
    remaining = packet_length - offset;
    payload_length = remaining < payload_per_fragment ? remaining : payload_per_fragment;
    total = ADS1299_BLE_FRAG_HEADER_SIZE + payload_length;
    if (out_capacity < total || payload_length > 255u) return 0u;

    out[0] = ADS1299_BLE_FRAG_MAGIC;
    put_u32le(&out[1], packet_sequence);
    out[5] = fragment_index;
    out[6] = (uint8_t)count;
    out[7] = (uint8_t)payload_length;
    memcpy(&out[ADS1299_BLE_FRAG_HEADER_SIZE], &packet[offset], payload_length);
    return total;
}

int ads1299_ble_parse_fragment(const uint8_t *fragment,
                               size_t fragment_length,
                               ads1299_ble_fragment_info_t *info,
                               const uint8_t **payload)
{
    if (!fragment || !info || !payload) return -1;
    if (fragment_length < ADS1299_BLE_FRAG_HEADER_SIZE) return -2;
    if (fragment[0] != ADS1299_BLE_FRAG_MAGIC) return -3;
    if (fragment[6] == 0u || fragment[6] > ADS1299_BLE_MAX_FRAGMENTS) return -4;
    if (fragment[5] >= fragment[6]) return -5;
    if ((size_t)fragment[7] + ADS1299_BLE_FRAG_HEADER_SIZE != fragment_length) return -6;

    info->packet_sequence = get_u32le(&fragment[1]);
    info->fragment_index = fragment[5];
    info->fragment_count = fragment[6];
    info->payload_length = fragment[7];
    *payload = &fragment[ADS1299_BLE_FRAG_HEADER_SIZE];
    return 0;
}

void ads1299_ble_reassembler_init(ads1299_ble_reassembler_t *state)
{
    if (state) memset(state, 0, sizeof(*state));
}

int ads1299_ble_reassembler_push(ads1299_ble_reassembler_t *state,
                                 const uint8_t *fragment,
                                 size_t fragment_length,
                                 uint8_t packet_out[ADS1299_PACKET_SIZE])
{
    ads1299_ble_fragment_info_t info;
    const uint8_t *payload;
    int rc;

    if (!state || !packet_out) return -1;
    rc = ads1299_ble_parse_fragment(fragment, fragment_length, &info, &payload);
    if (rc != 0) return rc;

    if (info.fragment_index == 0u) {
        if (state->next_fragment != 0u) state->dropped_packets++;
        state->packet_sequence = info.packet_sequence;
        state->expected_fragments = info.fragment_count;
        state->next_fragment = 0u;
        state->received_bytes = 0u;
    }

    if (state->expected_fragments == 0u ||
        info.packet_sequence != state->packet_sequence ||
        info.fragment_count != state->expected_fragments ||
        info.fragment_index != state->next_fragment) {
        state->dropped_packets++;
        state->expected_fragments = 0u;
        state->next_fragment = 0u;
        state->received_bytes = 0u;
        return -7;
    }

    if (state->received_bytes + info.payload_length > ADS1299_PACKET_SIZE) {
        state->dropped_packets++;
        state->expected_fragments = 0u;
        state->next_fragment = 0u;
        state->received_bytes = 0u;
        return -8;
    }

    memcpy(&state->packet[state->received_bytes], payload, info.payload_length);
    state->received_bytes += info.payload_length;
    state->next_fragment++;

    if (state->next_fragment == state->expected_fragments) {
        if (state->received_bytes != ADS1299_PACKET_SIZE ||
            ads1299_packet_validate(state->packet, ADS1299_PACKET_SIZE) != ADS1299_PACKET_OK) {
            state->dropped_packets++;
            state->expected_fragments = 0u;
            state->next_fragment = 0u;
            state->received_bytes = 0u;
            return -9;
        }
        memcpy(packet_out, state->packet, ADS1299_PACKET_SIZE);
        state->completed_packets++;
        state->expected_fragments = 0u;
        state->next_fragment = 0u;
        state->received_bytes = 0u;
        return 1;
    }

    return 0;
}
