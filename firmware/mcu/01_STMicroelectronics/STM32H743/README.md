# STM32H743ZIT6 ADS1299 Port

Global ecosystem rank: **1**. Status: **Compiles**. Tier A. Hardware validation is not implied.

## Platform

- Vendor: STMicroelectronics
- Family / MCU: STM32H7 / STM32H743ZIT6
- Architecture: Arm Cortex-M7F
- Reference board: NUCLEO-H743ZI2 (MB1364)
- Official environment: STM32CubeH7
- Compiler: GNU Arm Embedded via PlatformIO / STM32CubeIDE

## ADS1299 connection

Use SPI Mode 1 (CPOL=0, CPHA=1), MSB first. Keep CS software-controlled and
route DRDY, RESET, PWDN and START as independent GPIOs. Start at 4 MHz or less
until ID read, configuration readback and the internal test signal pass. The
reference pin assignment is documented in `board/pinmap.md`; confirm it against
the exact board revision before wiring.

## Repository layers

- ADS1299 behavior: `../../../core_driver/ads1299/`
- This platform's hardware-only adapter: `ads1299_port/`
- Minimal call flow: `examples/main_ads1299.c`
- Progressive beginner flow: `examples/stm32h743_beginner_demo.[ch]`
- Vendor-project procedure: `integration.md`

The port accepts SDK callbacks for SPI, GPIO and microsecond delay. It also
provides a millisecond helper without changing the stable Core port contract.
It never defines ADS1299 registers. UART, USB, BLE or Ethernet transport stays
in `firmware/transport/` and must not block a DRDY handler.


- STM32Cube binding: `stm32cube_adapter/`
- Reproducible compile record: `build.md`
- Official references and license provenance: `sources.md`

The checked reference is specifically the MB1364 NUCLEO-H743ZI2, not the older
MB1137 NUCLEO-H743ZI. D11 uses PA7 only with SB33 ON and SB35 OFF; changing
those solder bridges requires matching the SPI GPIO configuration.

## Reference-board-first CubeH7 quick start

`board/ads1299_board_config.h` is now the single hardware mapping file for the
STM32Cube path. A new project should not edit `ads1299.c`, `ads1299_regs.h` or
other shared Core files.

1. Create/open an STM32CubeIDE STM32H743 project and let CubeMX own startup,
   clock and linker files. Do not copy invented startup/linker files from this
   SDK.
2. Configure one SPI peripheral as master/full-duplex, 8-bit, MSB-first,
   CPOL Low, CPHA 2 Edge (SPI Mode 1), software NSS. For first bring-up keep
   SCLK at 4 MHz or below.
3. Configure ADS1299 CS, RESET, PWDN and START as output GPIOs and DRDY as an
   input (or falling-edge EXTI when advancing to interrupt-driven capture).
4. Prefer CubeMX user labels `ADS1299_CS`, `ADS1299_DRDY`, `ADS1299_RESET`,
   `ADS1299_PWDN`, `ADS1299_START` so the generated `*_GPIO_Port` and `*_Pin`
   names match the adapter directly.
5. Edit only `board/ads1299_board_config.h` if the SPI handle or generated GPIO
   labels differ. Missing mappings intentionally fail at compile time.
6. Add `examples/board_ads1299_hal_stm32cube.c`, the local `ads1299_port/`
   sources, and the shared `firmware/core_driver/ads1299/` sources used by the
   application. `examples/main_ads1299.c` now has a concrete implementation of
   its `board_ads1299_hal()` dependency through that adapter.
7. Build/flash with the Cube-generated project and use the minimal example to
   verify reset, ID probe and internal test before attaching electrodes or
   enabling a higher-throughput transport.

The Cube HAL adapter uses `HAL_SPI_TransmitReceive`, `HAL_GPIO_WritePin`,
`HAL_GPIO_ReadPin` and the Cortex-M7 DWT cycle counter for microsecond delays.
It supports full-duplex, TX-only and RX-only calls required by the portable MCU
port without moving ADS1299 register semantics into the board layer.

## Progressive beginner example

After the minimal reset/ID bring-up works, add
`examples/stm32h743_beginner_demo.c`, its header, and
`firmware/common/data_packet/ads1299_packet.c` to the CubeIDE application. The
example reuses the shared typed ADS1299 API and runs this fixed progression:

1. hardware reset + ADS1299 family ID probe;
2. internal-test configuration and eight captured validation frames;
3. input-short configuration and eight captured validation frames;
4. normal-input 250-SPS EEG configuration for the detected 4/6/8-channel
   variant;
5. optional finite canonical-packet streaming.

For the safest first run, call it without a transport:

```c
#include "stm32h743_beginner_demo.h"

int rc = stm32h743_ads1299_beginner_demo(NULL, 0u);
```

`stream_frames == 0` deliberately stops after the 250-SPS configuration, so a
new student can validate the ADS1299 path before writing UART/USB/Ethernet
code. To stream, provide `write()` and `timestamp_us()` in
`stm32h743_ads1299_demo_io_t` and request a finite frame count. Packets are
created by the shared `ads1299_packet_encode()` implementation; this platform
does not define a competing frame format or CRC.

Expected logger milestones, when a logger callback is supplied, are of the
form:

```text
ADS1299 STM32H743 beginner demo
OK ID ADS1299-8
RUN internal-test
OK internal-test frames
RUN input-short
OK input-short frames
RUN EEG 250 SPS config
OK EEG 250 SPS configured
OK stream skipped (stream_frames=0)
OK beginner demo complete
```

The detected ID line is `ADS1299-4`, `ADS1299-6`, or `ADS1299-8` as
appropriate. Error text intentionally separates RESET/power, SPI/CS/Mode-1,
DRDY, frame-read, and transport failures so initial bring-up does not require
editing the shared driver to discover the fault class.

### Reference hardware availability

ST currently marks the NUCLEO-H743ZI product page **Obsolete / Out of
Production**. Keep MB1364 NUCLEO-H743ZI2 as a reproducible legacy reference for
this exact STM32H743 model, but do not interpret this folder as a claim that a
new H743 Nucleo can still be purchased from ST. The STM32H743ZI MCU itself is
still an active device. For a custom/current STM32H743 board, the intended
migration surface is CubeMX plus `board/ads1299_board_config.h` only.

### Validation status for the newly added Cube adapter and beginner flow

- Board/config edit point: **PRESENT**.
- STM32Cube HAL implementation of `board_ads1299_hal()`: **PRESENT**.
- Progressive probe/internal-test/input-short/250-SPS/stream source: **PRESENT**.
- Canonical packet encoder reuse: **PRESENT; shared implementation**.
- Shared ADS1299 register/control logic: **REUSED; not duplicated here**.
- Existing host integration regression suite: **PRESENT**.
- Newly added Cube adapter and beginner example target build: **not yet BUILD-VERIFIED**.
- NUCLEO-H743ZI2 + ADS1299 physical execution: **not BOARD-VERIFIED**.
- DMA/cache sustained streaming: **not verified**.

The historical `Status: Compiles` metadata at the top predates this newly added
Cube board adapter and beginner flow; it must not be read as a build-verification
claim for these additions until a documented target build is actually recorded.
