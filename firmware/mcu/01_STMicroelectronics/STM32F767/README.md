# STM32F767 ADS1299 Port

Global ecosystem rank: **107**. Current round status: **integration-candidate-complete**. Hardware validation is not implied.

## Reference platform

- Vendor: STMicroelectronics
- MCU: STM32F767ZIT6
- Reference board: **NUCLEO-F767ZI (MB1137)**
- Tooling: STM32CubeMX + STM32CubeIDE + STM32CubeF7 HAL
- Shared ADS1299 core: `../../../core_driver/ads1299/`

The reference board is the reproducible starter path for this model folder. Other STM32F767 boards are expected to reuse the same shared core and normally require only CubeMX routing, `board/board_config.h`, and peripheral-handle binding changes.

## Beginner path

1. Read `board/README.md` and wire the ADS1299 to the documented NUCLEO-F767ZI pins.
2. Configure SPI1 as Mode 1 (CPOL=0, CPHA=1), 8-bit, MSB-first, software CS.
3. Configure USART3/ST-LINK VCP at the baud rate defined by `board/board_config.h` for canonical streaming.
4. Add the model-local port/example files plus the shared ADS1299 core to the Cube project; exact ownership is listed in `integration.md`.
5. Call `stm32f767_ads1299_beginner_demo(1000u)` after Cube-generated peripheral initialization.
6. Verify probe/ID, internal test, input-short, 250-SPS EEG configuration, canonical packet streaming and clean STOP/SDATAC.
7. Use `0u` only when an endless stream is intentionally required.

The beginner path does **not** require editing `ads1299.c`, `ads1299_regs.h`, `ads1299_model.c` or any other shared ADS1299 core file.

## Repository layers

- `board/board_config.h`: single repository-owned board/config entry point.
- `ads1299_port/`: STM32F767 SPI/GPIO/DRDY adapter only.
- `examples/stm32f767_beginner_demo.c`: progressive beginner flow.
- `examples/stm32f767_example_platform.c`: concrete STM32F7 HAL binding and legacy compatibility binding.
- `tests/`: host-side STM32F767 port + shared-core smoke tests.
- `integration.md`: exact Cube integration/build/run procedure.
- `validation.md`: evidence status; do not infer hardware verification from source presence.

Shared ADS1299 behavior stays under `firmware/core_driver/ads1299/`. This model folder must not duplicate register/profile/frame logic.

## What success looks like

The starter log should progress through messages equivalent to:

```text
ADS1299 STM32F767 beginner demo
OK ID ADS1299-8
RUN internal-test
OK internal-test frames
RUN input-short
OK input-short frames
RUN EEG 250 SPS config
OK EEG 250 SPS configured
RUN stream canonical packets
```

ADS1299-4 and ADS1299-6 IDs are also accepted by the example path.

## Sustained acquisition rule

The blocking HAL path is intentionally a first-bring-up path. For sustained acquisition, higher rates, or multiple ADS1299 devices, move the DRDY timing path to IRQ/DMA and a bounded queue/ring buffer with explicit overflow accounting. UART/USB/Ethernet work must not block DRDY handling. On STM32F7, DMA buffers placed in cacheable SRAM require an explicit cache-coherency strategy.

The repository default multi-device architecture remains shared SPI with independent software-controlled CS per ADS1299 unless another topology is intentionally documented and verified.

## Validation status

**Integration candidate-complete for the current round.** Board/config, HAL/portable port, progressive beginner example, diagnostics and a host integration-test recipe are present. The repository does **not** currently record a successful host test execution, STM32CubeIDE clean build, physical NUCLEO-F767ZI + ADS1299 run, sustained-stream validation, DMA/cache validation, multi-ADS1299 validation or 64-channel validation.

## Legacy compatibility

The original `examples/main_ads1299.c` path remains present. `stm32f767_example_platform.c` supplies its `board_ads1299_hal()` compatibility binding, so existing user work is preserved while new students use the progressive demo.

Maintenance identifier 107 is a repository traversal identifier, not a sales ranking or supply guarantee.
