/* Compile-smoke contract for a consuming MIMXRT1189 board binding.
 *
 * Add this source beside the board project's board_ads1299_hal()
 * implementation. It deliberately contains no vendor startup/linker code and
 * performs no hardware I/O; its purpose is to make the board/core boundary
 * explicit and catch a missing or incompatible binding at build/link time.
 */
#include "board_ads1299.h"

int ads1299_mimxrt1189_board_binding_check(void)
{
    ads1299_platform_hal_t hal;

    /* Referencing the public binding here ensures its declaration remains
     * type-compatible with the portable MCU port contract. The consuming
     * project supplies the actual definition. */
    return board_ads1299_hal(&hal);
}
