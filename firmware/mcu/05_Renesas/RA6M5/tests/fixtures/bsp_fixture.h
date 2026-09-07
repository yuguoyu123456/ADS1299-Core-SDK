/* TEST ONLY. Bypasses MCU BSP; external official FSP API headers stay intact.
 * This is not an RA6M5 build configuration and must never ship in firmware. */
#ifndef ADS1299_TEST_BSP_FIXTURE_H
#define ADS1299_TEST_BSP_FIXTURE_H
#define BSP_API_H
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#define FSP_HEADER
#define FSP_FOOTER
typedef int fsp_err_t;
#define FSP_SUCCESS 0
typedef int IRQn_Type;
typedef uint32_t bsp_io_port_pin_t;
typedef uint32_t bsp_io_port_t;
typedef enum { BSP_IO_LEVEL_LOW, BSP_IO_LEVEL_HIGH } bsp_io_level_t;
#endif
