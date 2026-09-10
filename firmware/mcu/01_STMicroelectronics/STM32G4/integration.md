# Integration with STM32CubeG4

1. Install the official `STM32CubeG4` environment; do not copy the vendor SDK into this repository.
2. Create or open a vendor project for `STM32G474RE` / `NUCLEO-G474RE; provisional control GPIO profile`.
3. Add `firmware/core_driver/ads1299/*.c` and its include path.
4. Add this directory's `ads1299_port/*.c` and include path.
5. Implement `board_ads1299_hal()` using the vendor SDK's SPI, GPIO and microsecond-delay APIs.
6. Configure SPI as Mode 1, MSB first, conservative clock, software CS.
7. Copy the call flow from `examples/main_ads1299.c`; keep product transport outside the example.
8. First verify power, ID read, register readback and internal test input. Only then connect electrodes.

The `spi_transfer` callback is full duplex. A null TX pointer means clock zero
bytes while receiving; a null RX pointer means discard received bytes. If the
vendor SDK rejects null buffers, implement that behavior with a small scratch
buffer in `board_ads1299_hal()`. CS is controlled by the separate GPIO callback
and must not be toggled inside `spi_transfer`.

## SDK ownership boundary

Vendor startup, linker scripts, CMSIS/HAL, generated configuration and middleware
stay in the user's official SDK project. This repository owns only the thin
callback adapter and ADS1299-independent tests. Pin `CubeG4 1.5.2 linker; CMSIS device 626ee412; GNU reference verified` in the
consumer project and record any API change in `version.md`.

## Concrete CMSIS binding

Add cmsis_adapter/ads1299_g4_hal.c, standard Port and shared Core to your official Cube project. Use either your own board binding or board/reference_image.c, never both. The reference requires HSI16 reset clocks and owns SPI1; do not also initialize it through HAL. See board/reference_image.md.

---

## Current standardized CubeG4 beginner integration

The older callback recipe above remains preserved for compatibility. New student bring-up should use the already-present standardized model-local board/port/example layers rather than implementing another competing ADS1299 register path.

### 1. Create the official target project

Create a normal STM32CubeIDE / STM32CubeG4 project for **NUCLEO-G474RE / STM32G474RET6**. Keep Cube-generated startup, linker script, system clock, HAL and CMSIS files in that project.

### 2. Configure the reference peripherals

Use the standardized mapping from `board/board_config.h` and `board/README.md`:

- SPI1 master: PA5 SCK, PA6 MISO, PA7 MOSI;
- SPI data size: 8 bit;
- bit order: MSB first;
- CPOL Low, CPHA 2 Edge = ADS1299 SPI Mode 1;
- NSS/CS: software controlled, PB6 GPIO output idle high;
- PC7: DRDY input, active low; polling is acceptable only for first bring-up;
- PA9: RESET output;
- PA8: PWDN output;
- PB10: START output;
- LPUART1 PA2/PA3: VCP transport for the starter example;
- starter transport: 460800 baud, 8-N-1.

Use a conservative SPI clock for first ID/probe and internal-test validation. Do not hide CS transitions inside the SPI transfer callback.

### 3. Add repository-owned model-local sources

Add:

- `ads1299_port/ads1299_spi.c`
- `ads1299_port/ads1299_gpio.c`
- `ads1299_port/ads1299_drdy.c`
- `examples/stm32g4_example_platform.c`
- `examples/stm32g4_beginner_demo.c`

Keep the old `examples/main_ads1299.c` only when you intentionally want the legacy debugger-style flow.

### 4. Add shared ADS1299 sources

Add the shared ADS1299 core from `firmware/core_driver/ads1299/` using the repository's current shared build metadata/file inventory, plus:

- `firmware/common/data_packet/ads1299_packet.c`

Add include paths for the model `board/`, `ads1299_port/`, `examples/`, shared ADS1299 core and `firmware/common/data_packet/`.

Do not copy ADS1299 register definitions or profile bytes into this STM32G4 directory.

### 5. Application entry

After `HAL_Init()`, system-clock initialization, GPIO, SPI1 and LPUART1 initialization, call:

```c
stm32g4_ads1299_beginner_demo(1000u);
```

A finite run should progress through:

`reset -> SDATAC -> ID/probe -> internal-test -> input-short -> 250-SPS gain-24 EEG -> canonical stream -> STOP/SDATAC`

Pass `0u` only when continuous streaming is desired.

### 6. Expected diagnostics

The readable stage before binary packets should distinguish at least:

- reset/control GPIO failure;
- SDATAC/SPI/ID failure;
- DRDY timeout;
- frame-read failure;
- host-transport failure.

A detected ADS1299-4, -6 or -8 should be handled according to the shared model layer rather than by hard-coded register scripts in this platform folder.

### 7. Sustained acquisition rule

The blocking starter path is intentionally simple. For sustained acquisition, move to:

`short DRDY/EXTI ISR -> static bounded queue/ring buffer -> non-real-time packetization/transport`

If DMA is introduced, keep frame boundaries and software CS deterministic, make cache/buffer ownership explicit where relevant, and expose overflow counters rather than silently dropping frames.

## Validation boundary

This standardized integration is **integration candidate-complete**, but a successful STM32CubeIDE build has not yet been recorded here and no physical NUCLEO-G474RE + ADS1299 run is claimed. The preserved CMSIS/reference-image experiments do not imply BOARD-VERIFIED status for the standardized beginner path.
