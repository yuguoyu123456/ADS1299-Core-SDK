# Integration with STM32Cube / STM32CubeF4

## Reference integration path

Reference board: **STM32F429I-DISC1 (MB1075)** with **STM32F429ZIT6**.

1. Install STM32CubeMX / STM32CubeIDE and the STM32CubeF4 package; do not copy the vendor SDK into this repository.
2. Create or open a project for STM32F429I-DISC1 / STM32F429ZIT6.
3. Reproduce the board routing from `board/board_config.h` and `board/README.md` in CubeMX.
4. Configure SPI1 master, full duplex, **Mode 1 (CPOL=0, CPHA=1), 8-bit, MSB-first, software NSS**.
5. Configure CS, RESET, PWDN and START as GPIO outputs, DRDY as an input, and USART1 if using the starter VCP stream.
6. Add shared ADS1299 core sources from `../../../core_driver/ads1299/` and its include path.
7. Add this model's `ads1299_port/*.c` and include path.
8. Add `examples/stm32f429_example_platform.c`, `examples/stm32f429_beginner_demo.c` and their headers.
9. After the Cube-generated peripheral initialization, call `stm32f429_ads1299_beginner_demo(1000u)`; pass `0u` for continuous streaming.
10. Verify the staged flow: ID/probe, internal-test, input-short, 250-SPS EEG, packet streaming and clean stop. Connect human electrodes only after the electrical design and acquisition chain have been independently validated for the intended use.

## What a beginner edits

`board/board_config.h` is the repository-owned board/config edit point. The generated Cube project owns clock-tree, alternate-function, peripheral-handle and startup configuration. A different STM32F429 board should normally require changes only in those board/Cube layers, not in `ads1299.c`, `ads1299_regs.h`, `ads1299_model.c` or other shared ADS1299 behavior.

The earlier scaffold expected users to implement a generic `board_ads1299_hal()` callback manually. That compatibility path remains available through the legacy example, but it is no longer the recommended beginner route; `stm32f429_example_platform.c` provides the concrete STM32F4 HAL binding for the progressive example.

## SPI callback contract

The `spi_transfer` callback is full duplex. A null TX pointer means clock zero bytes while receiving; a null RX pointer means discard received bytes. If a consuming HAL configuration cannot accept null buffers directly, the platform adapter must preserve those semantics locally. CS is controlled separately and must not be toggled inside the portable transfer contract.

## Streaming and timing

A blocking starter path is acceptable for single-device 250-SPS bring-up. For sustained high-rate acquisition or multiple ADS1299 devices, keep DRDY servicing bounded: use DMA/interrupt-driven acquisition and a bounded queue/ring buffer, move host transport out of the timing-critical path, and count/report overflow explicitly.

## Host smoke tests

From `tests/`, the repository now provides a host integration build recipe that compiles the STM32F429 port sources together with shared ADS1299 core modules:

```sh
make -f Makefile.host test
```

The existence of this recipe is not itself a recorded PASS. Record actual compiler/toolchain evidence in `validation.md` when executed.

## SDK ownership boundary

Vendor startup, linker scripts, CMSIS/HAL, generated configuration and middleware stay in the user's official STM32 project. This repository owns the board/config contract, thin STM32F429 port/HAL binding, progressive ADS1299 example, tests, and shared ADS1299-independent behavior. Pin the consuming STM32CubeF4 version in the application project and record material API changes in `version.md`.
