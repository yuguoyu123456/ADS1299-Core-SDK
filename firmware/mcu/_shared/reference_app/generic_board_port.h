#ifndef ADS1299_GENERIC_BOARD_PORT_H
#define ADS1299_GENERIC_BOARD_PORT_H

#include "ads1299_reference_app.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Implemented by generic_board_port.c after including the project's
 * port/board_sdk.h. */
const ads1299_reference_board_t *ads1299_board_instance(void);

#ifdef __cplusplus
}
#endif

#endif /* ADS1299_GENERIC_BOARD_PORT_H */
