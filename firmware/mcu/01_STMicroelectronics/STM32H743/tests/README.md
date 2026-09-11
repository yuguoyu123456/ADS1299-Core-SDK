# STM32H743 ADS1299 host regression tests

This folder provides host-side smoke/regression coverage for the STM32H743 ADS1299 integration boundary without claiming physical-board validation.

## Run

From this directory:

```sh
make -f Makefile.host clean
make -f Makefile.host test
```

The unified runner compiles the STM32H743 port together with the shared ADS1299 core and canonical packet implementation, then executes:

- 24-bit ADS1299 frame numeric boundary checks;
- register/core boundary smoke checks;
- SPI port behavior;
- RESET/delay behavior;
- DRDY active-low behavior;
- canonical packet encode/validate/decode and CRC-corruption rejection.

Expected final line after a successful host run:

```text
PASS STM32H743 integration smoke tests
```

## Validation status

- Host build recipe: PRESENT.
- Unified host runner: PRESENT.
- DRDY regression: PRESENT.
- Canonical packet/CRC regression: PRESENT.
- Actual host PASS for the current revision: not claimed unless a run is recorded.
- STM32H743 target firmware BUILD-VERIFIED: NO.
- Physical STM32H743 board + ADS1299 BOARD-VERIFIED: NO.

These tests deliberately reuse `firmware/core_driver/ads1299` and `firmware/common/data_packet`; they do not duplicate ADS1299 register or packet logic inside the STM32H743 model folder.
