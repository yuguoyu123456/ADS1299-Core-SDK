# STM32H743 ADS1299 host regression tests

This folder provides host-side smoke/regression coverage for the STM32H743 ADS1299 integration boundary without claiming physical-board validation.

## Run

From this directory:

```sh
make -f Makefile.host clean
make -f Makefile.host test
```

The unified runner compiles the STM32H743 port, progressive beginner example and bounded acquisition queue together with the shared ADS1299 core and canonical packet implementation, then executes:

- 24-bit ADS1299 frame numeric boundary checks;
- register/core boundary smoke checks;
- SPI port behavior;
- RESET/delay behavior;
- DRDY active-low behavior;
- canonical packet encode/validate/decode and CRC-corruption rejection;
- beginner-demo compile/link/API-integrity smoke coverage;
- bounded frame-queue FIFO, wraparound, overflow, argument validation and high-watermark diagnostics;
- 4096-frame interleaved producer/consumer stress regression with sequence, timestamp, status and all channel values checked.

Expected final line after a successful host run:

```text
PASS STM32H743 integration smoke tests
```

## Validation status

- Host build recipe: PRESENT.
- Unified host runner: PRESENT.
- DRDY regression: PRESENT.
- Canonical packet/CRC regression: PRESENT.
- Beginner-demo host smoke: PRESENT.
- Bounded queue regression and 4096-frame stress test: PRESENT and executed by the unified runner.
- Actual host PASS for the current revision: not claimed unless a run is recorded.
- STM32H743 target firmware BUILD-VERIFIED: NO.
- Physical STM32H743 board + ADS1299 BOARD-VERIFIED: NO.
- STM32H7 DMA/cache sustained-acquisition validation: NO.

These tests deliberately reuse `firmware/core_driver/ads1299` and `firmware/common/data_packet`; they do not duplicate ADS1299 register or packet logic inside the STM32H743 model folder.
