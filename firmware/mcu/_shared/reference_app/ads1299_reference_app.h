#ifndef ADS1299_REFERENCE_APP_H
#define ADS1299_REFERENCE_APP_H

#include <stddef.h>
#include <stdint.h>

#include "ads1299.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const char *name;
    void *user;
    int (*init)(void *user);
    ads1299_port_t (*make_port)(void *user);
    int (*stream_write)(void *user, const uint8_t *data, size_t len);
    void (*fatal)(void *user, const char *reason);
} ads1299_reference_board_t;

typedef struct {
    const ads1299_reference_board_t *board;
    ads1299_t ads;
    uint32_t sequence;
    uint32_t sample_period_us;
} ads1299_reference_app_t;

/* Initialize one deterministic first-boot profile: hardware reset, ID read,
 * 250 SPS, gain 24, internal test source, RDATAC, START. */
ads1299_status_t ads1299_reference_app_init(
    ads1299_reference_app_t *app,
    const ads1299_reference_board_t *board);

/* Wait for one DRDY, acquire one complete frame and emit one repository common
 * packet. The first-boot profile uses a fixed 4000-us sample timeline. */
ads1299_status_t ads1299_reference_app_step(ads1299_reference_app_t *app);

#ifdef __cplusplus
}
#endif

#endif /* ADS1299_REFERENCE_APP_H */
