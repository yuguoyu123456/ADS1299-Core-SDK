#ifndef ADS1299_BLE_FRAG_H
#define ADS1299_BLE_FRAG_H

#include <stddef.h>
#include <stdint.h>
#include "ads1299_packet.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ADS1299_BLE_FRAG_MAGIC 0xB9u
#define ADS1299_BLE_FRAG_HEADER_SIZE 8u
#define ADS1299_BLE_MAX_FRAGMENTS 16u

typedef struct {
    uint32_t packet_sequence;
    uint8_t fragment_index;
    uint8_t fragment_count;
    uint8_t payload_length;
} ads1299_ble_fragment_info_t;

typedef struct {
    uint8_t packet[ADS1299_PACKET_SIZE];
    uint32_t packet_sequence;
    uint8_t expected_fragments;
    uint8_t next_fragment;
    size_t received_bytes;
    uint32_t completed_packets;
    uint32_t dropped_packets;
} ads1299_ble_reassembler_t;

size_t ads1299_ble_fragment_count(size_t packet_length, size_t att_payload_max);
size_t ads1299_ble_make_fragment(const uint8_t *packet,
                                 size_t packet_length,
                                 uint32_t packet_sequence,
                                 size_t att_payload_max,
                                 uint8_t fragment_index,
                                 uint8_t *out,
                                 size_t out_capacity);
int ads1299_ble_parse_fragment(const uint8_t *fragment,
                               size_t fragment_length,
                               ads1299_ble_fragment_info_t *info,
                               const uint8_t **payload);
void ads1299_ble_reassembler_init(ads1299_ble_reassembler_t *state);
int ads1299_ble_reassembler_push(ads1299_ble_reassembler_t *state,
                                 const uint8_t *fragment,
                                 size_t fragment_length,
                                 uint8_t packet_out[ADS1299_PACKET_SIZE]);

#ifdef __cplusplus
}
#endif

#endif
