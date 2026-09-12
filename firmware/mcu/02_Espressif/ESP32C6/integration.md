# Integration with ESP-IDF

1. Install the official `ESP-IDF` environment; do not copy the vendor SDK into this repository.
2. Create or open a vendor project for `ESP32-C6` / `ESP32-C6-DevKitC-1`.
3. Add `firmware/core_driver/ads1299/*.c` and its include path.
4. Add this directory's `ads1299_port/*.c`, `esp_idf_adapter/*.c`, `board/esp_idf_board.c`, `examples/esp32c6_frame_queue.c` and include paths.
5. Keep `board_ads1299_hal()` as the only board/SDK-specific ownership point for SPI, GPIO and timing.
6. Configure SPI as Mode 1, MSB first, conservative clock and software CS.
7. Use `examples/main_ads1299.c` as the maintained progressive flow rather than duplicating ADS1299 register setup.
8. Verify probe/ID, internal-test and input-short before connecting electrodes; then proceed to the 250-SPS EEG stream.

The `spi_transfer` callback is full duplex. A null TX pointer means clock zero
bytes while receiving; a null RX pointer means discard received bytes. If the
vendor SDK rejects null buffers, implement that behavior with a small scratch
buffer in `board_ads1299_hal()`. CS is controlled by the separate GPIO callback
and must not be toggled inside `spi_transfer`.

## Beginner execution sequence

The maintained reference example performs:

```text
board/HAL init
  -> shared ADS1299 init
  -> hardware reset + SDATAC
  -> ADS1299-family probe
  -> internal test (8 diagnostic frames)
  -> STOP + SDATAC
  -> input-short (8 diagnostic frames)
  -> STOP + SDATAC
  -> 250 SPS + gain 24 + normal inputs + SRB1 off
  -> RDATAC + START
  -> acquisition task -> bounded queue -> transport task
```

The acquisition task is deliberately higher priority than the transport task.
It performs DRDY polling, one complete frame read, timestamp/sequence capture
and queue push only. Console formatting, UART, Wi-Fi, BLE or Thread transport
belongs in the lower-priority consumer. The queue reports depth, high-watermark
and dropped frames so overload is explicit.

`ads1299_latest_frame` and `ads1299_frame_sequence` remain available as a
backward-compatible debugger snapshot; they are not the transport contract.

## Failure classes

The reference logs distinguish at least:

- board/HAL or portable-port initialization failure;
- reset/SDATAC or ADS1299 ID/probe failure;
- internal-test configuration, DRDY or frame-read failure;
- input-short configuration, DRDY or frame-read failure;
- 250-SPS/SRB/channel configuration failure;
- RDATAC/START failure;
- acquisition DRDY/frame-read failure;
- queue overflow/back-pressure;
- FreeRTOS task-creation failure.

## SDK ownership boundary

Vendor startup, linker scripts, generated configuration and middleware stay in
the user's official ESP-IDF project. This repository owns the thin platform
adapter, board configuration, portable shared ADS1299 driver integration and
host-testable queue. Pin the documented ESP-IDF/toolchain version in the
consumer project and record API changes in `version.md`.

## Validation boundary

The source tree now contains the progressive diagnostic flow and bounded
continuous task split, but source presence is not hardware evidence. Do not
claim **BUILD-VERIFIED** until the documented ESP-IDF reference build actually
succeeds. Do not claim **BOARD-VERIFIED** until the stated physical board and
ADS1299 hardware have actually completed probe, test, 250-SPS streaming and
back-pressure checks. See `build.md` for the current build procedure and
validation scope.
