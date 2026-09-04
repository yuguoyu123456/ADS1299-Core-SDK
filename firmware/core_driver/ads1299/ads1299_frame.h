#ifndef ADS1299_FRAME_H
#define ADS1299_FRAME_H

#include <stddef.h>
#include <stdint.h>

#include "ads1299.h"
#include "ads1299_register_model.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t header_nibble;      /* Must be 0xC for a valid ADS1299 status word. */
    uint8_t loff_statp;         /* Positive lead-off status bits, CH1 = bit0. */
    uint8_t loff_statn;         /* Negative lead-off status bits, CH1 = bit0. */
    uint8_t gpio_data;          /* GPIO1..GPIO4 state in bits0..3. */
    uint8_t header_valid;
} ads1299_frame_status_t;

/** Return the expected byte count for one conversion frame of a 4/6/8-ch part. */
size_t ads1299_frame_bytes_for_variant(ads1299_variant_t variant);

/** Decode the TI 24-bit status field: 1100 + LOFF_STATP + LOFF_STATN + GPIO[7:4]. */
ads1299_status_t ads1299_decode_frame_status(const uint8_t raw_status[ADS1299_STATUS_BYTES],
                                             ads1299_variant_t variant,
                                             ads1299_frame_status_t *status);

/**
 * Decode a complete conversion frame for the selected ADS1299-x variant.
 * The destination always has room for eight channels; unused channels are zeroed.
 */
ads1299_status_t ads1299_decode_frame_variant(const uint8_t *raw,
                                              size_t raw_len,
                                              ads1299_variant_t variant,
                                              ads1299_frame_t *frame,
                                              ads1299_frame_status_t *status);

#ifdef __cplusplus
}
#endif

#endif /* ADS1299_FRAME_H */
