# ESP32-C6 binding in progress

Single-task, dedicated SPI2 bus, Mode 1, MSB-first, software CS, queue size 1,
DMA disabled and <=32-byte internal transactions. Board owns configuration and
must prevent bus sharing for the entire Core CS-low interval, not just each chunk.

Use a zero-initialized static context and finite timeout_ticks. Both descriptor
and buffers survive completion timeout. A timeout latches fault and retains
in_flight; do not reuse/free the context or remove the SPI device until the driver
returns the descriptor. Recovery/driver teardown is intentionally not automatic.
Caller RX memory is touched only after successful completion. GPIO errors latch
fault and subsequent DRDY reads return negative. This is task context only.

Reference API contract:
[official ESP-IDF SPI master documentation](https://docs.espressif.com/projects/esp-idf/en/release-v5.5/esp32c6/api-reference/peripherals/spi_master.html).
The queued descriptor must remain unchanged until returned by the driver.
No vendor source was copied. Actual IDF 5.4.0 bootloader/application build and
mock queue tests passed; see build.md. Hardware execution remains unverified,
and the catalog conservatively remains Reference.
