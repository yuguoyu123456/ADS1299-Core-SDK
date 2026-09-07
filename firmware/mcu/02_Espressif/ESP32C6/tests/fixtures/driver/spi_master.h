/* TEST ONLY: mock queue interface, not the IDF ABI. */
#ifndef TEST_SPI_H
#define TEST_SPI_H
#include <stdint.h>
#include <stddef.h>
typedef uint32_t TickType_t;
#define portMAX_DELAY UINT32_MAX
#define ESP_OK 0
typedef void *spi_device_handle_t;
typedef struct { size_t length; const void *tx_buffer; void *rx_buffer; } spi_transaction_t;
int spi_device_queue_trans(spi_device_handle_t,spi_transaction_t *,TickType_t);
int spi_device_get_trans_result(spi_device_handle_t,spi_transaction_t **,TickType_t);
#endif
