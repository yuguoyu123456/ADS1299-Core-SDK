# STM32F407 ADS1299 host integration tests

This directory contains host-side smoke tests for the STM32F407 ADS1299 platform layer together with the shared portable ADS1299 core and canonical packet layer.

## Scope

The test executable covers:

- shared 24-bit ADS1299 frame sign-extension boundaries;
- register/API opacity boundary checks already present in this model folder;
- SPI callback forwarding and invalid-argument handling;
- RESET/GPIO/delay forwarding;
- ADS1299 DRDY active-low semantics through both `ads1299_mcu_data_ready()` and the portable `ads1299_port_t.drdy_read()` callback;
- canonical ADS1299 packet encode/validate/decode behavior using the shared `firmware/common/data_packet/ads1299_packet.c` implementation;
- sync/version/flags, sequence, timestamp, 3-byte status and 8-channel payload preservation;
- CRC rejection after a deliberate one-bit payload corruption.

The build deliberately compiles the STM32F407 platform sources together with the shared `ads1299.c`, `ads1299_frame.c`, `ads1299_model.c`, `ads1299_multi.c`, and `ads1299_packet.c`. This helps catch core/port/transport integration regressions without requiring STM32 hardware, while keeping packet framing and CRC logic out of the model folder.

## Run on a host computer

From this directory:

```sh
make -f Makefile.host clean
make -f Makefile.host test
```

Expected final line:

```text
PASS STM32F407 integration smoke tests
```

Clean the generated executable with:

```sh
make -f Makefile.host clean
```

## Validation meaning

Source coverage in this folder now includes the canonical packet/CRC regression, but no host PASS is claimed unless the command above is actually executed successfully in a suitable environment.

A passing host test would mean only that these portable/platform C sources compile and the mocked callback plus packet behavior passes this smoke suite. It does **not** mean that STM32CubeIDE compilation, STM32F407G-DISC1 hardware operation, ADS1299 electrical behavior, sustained streaming, DMA performance, or multi-ADS1299 acquisition has been verified.

Hardware-dependent validation remains separate and must be recorded explicitly before claiming BUILD-VERIFIED or BOARD-VERIFIED status.
