#ifndef STM32H743_ADS1299_BEGINNER_DEMO_H
#define STM32H743_ADS1299_BEGINNER_DEMO_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    /* Optional human-readable logger. May be NULL. */
    void (*log)(const char *message);

    /* Required only when stream_frames is non-zero. Return 0 on success. */
    int (*write)(const uint8_t *data, size_t len);

    /* Required only when canonical streaming is requested. */
    uint64_t (*timestamp_us)(void);
} stm32h743_ads1299_demo_io_t;

/*
 * Run the complete beginner path:
 *   probe/ID -> internal test -> input short -> 250-SPS EEG -> stream.
 *
 * stream_frames == 0 performs probe + diagnostics + 250-SPS configuration
 * without entering an unbounded stream. A positive value streams exactly that
 * many canonical repository packets.
 *
 * The board-dependent SPI/GPIO mapping remains in
 * ../board/ads1299_board_config.h. ADS1299 register semantics remain in the
 * shared core.
 */
int stm32h743_ads1299_beginner_demo(const stm32h743_ads1299_demo_io_t *io,
                                    uint32_t stream_frames);

#ifdef __cplusplus
}
#endif

#endif /* STM32H743_ADS1299_BEGINNER_DEMO_H */
