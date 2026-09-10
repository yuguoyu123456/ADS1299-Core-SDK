# STM32H533 ADS1299 port

This folder adapts STM32H533 board/HAL operations to the shared portable `ads1299_port_t` interface. ADS1299 register programming, frame parsing, profiles and multi-device behavior belong in `firmware/core_driver/ads1299/`; do not duplicate them here.

## What belongs here

- SPI byte transfer callback forwarding.
- CS, RESET, PWDN and START GPIO forwarding.
- DRDY input handling. ADS1299 DRDY is active-low: `ads1299_mcu_data_ready()` returns 1 when the physical pin is low, while the portable `ads1299_port_t.drdy_read()` callback returns the physical logic level expected by the shared core (0 means ready).
- Microsecond delay forwarding.

Board-specific pins, peripheral instances, clocks, DMA channels and transport choices should live in the STM32H533 `board/` layer rather than in the shared ADS1299 core.

## Host self-test

A host-only fake HAL test verifies the port contract without STM32 hardware:

```sh
make -f Makefile.host clean
make -f Makefile.host test
```

Expected final line:

```text
PASS STM32H533 ads1299_port host self-test
```

The test checks incomplete-HAL rejection, SPI forwarding and invalid arguments, control GPIO routing, active-low DRDY semantics, construction of `ads1299_port_t`, and delay forwarding.

This test does **not** verify STM32H533Cube HAL integration, SPI timing on a physical board, DMA behavior, ADS1299 electrical connectivity, or sustained acquisition.

## STM32H533 acquisition guidance

For first bring-up a blocking HAL transfer path is acceptable. For sustained EEG acquisition, keep the DRDY interrupt path short and deterministic: capture/queue work only, then perform packetization and host transport outside the timing-critical path. Use a bounded static queue/ring buffer with explicit overflow accounting.

If SPI DMA is enabled, preserve deterministic CS framing around each ADS1299 transaction and do not reuse or overwrite DMA buffers until completion is confirmed. On cache-enabled configurations, place DMA buffers in an appropriate memory region or perform the cache maintenance required by the selected STM32H533 memory/cache configuration. These are platform-integration responsibilities; they must not change ADS1299 register semantics in the shared core.

## Validation status

- Port source: **PRESENT**.
- Host self-test source and build recipe: **PRESENT**.
- Host self-test execution result: **NOT RECORDED in this repository yet**.
- STM32H533 target build: **not BUILD-VERIFIED here**.
- Physical STM32H533 + ADS1299 hardware: **not BOARD-VERIFIED here**.

Do not infer bench testing, long-run stability, DMA correctness, electrical safety, EMC, production readiness or regulatory compliance from the presence of this port layer.
