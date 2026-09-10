# STM32G4 ADS1299 examples

Reference path: **NUCLEO-G474RE (MB1367 / STM32G474RET6)** with STM32CubeG4 HAL.

Validation status: **TEMPLATE / repository integration present**. The sources below have not yet been recorded as STM32CubeIDE BUILD-VERIFIED or physical BOARD-VERIFIED.

## Recommended beginner entry point

Call:

```c
stm32g4_ads1299_beginner_demo(1000u);
```

from your Cube-generated application after `HAL_Init()`, clock setup, GPIO setup, `MX_SPI1_Init()` and `MX_LPUART1_UART_Init()` have completed. Passing `0u` streams until reset/power-off.

The progressive demo performs:

1. hardware reset + SDATAC;
2. ADS1299-family ID/probe;
3. internal-test acquisition;
4. input-short acquisition;
5. 250-SPS, gain-24 normal-input configuration;
6. continuous DRDY/read;
7. canonical 49-byte packet streaming;
8. clean STOP + SDATAC for finite runs.

The older `main_ads1299.c` debugger-snapshot example is intentionally retained for compatibility.

## The only normal board file to edit

For the repository reference mapping, use:

`../board/board_config.h`

A different STM32G4 board should normally require only CubeMX peripheral/pin changes plus corresponding values in that board/config layer. Do **not** edit shared `ads1299.c`, `ads1299_regs.h`, `ads1299_model.c` or packet internals for first bring-up.

## Reference Cube configuration

Configure the NUCLEO-G474RE project with:

- SPI1 master, 8-bit, MSB first;
- CPOL = Low and CPHA = 2 Edge (**SPI Mode 1**);
- PA5=SCK, PA6=MISO, PA7=MOSI;
- PB6=CS GPIO output, idle high;
- PC7=DRDY GPIO input (or EXTI later for a non-blocking acquisition path);
- PA9=RESET GPIO output;
- PA8=PWDN GPIO output;
- PB10=START GPIO output;
- LPUART1 on PA2/PA3 using the default ST-LINK VCP routing;
- starter stream baud = 460800, 8-N-1.

The blocking example is for deterministic first bring-up. Sustained acquisition should move DRDY handling to a short ISR and use a bounded static queue/ring buffer before packetization/host transport. If SPI DMA is introduced, preserve deterministic CS/frame boundaries and measure overflow rather than silently dropping frames.

## Files to add to the Cube project

Model-local sources:

- `../ads1299_port/ads1299_spi.c`
- `../ads1299_port/ads1299_gpio.c`
- `../ads1299_port/ads1299_drdy.c`
- `stm32g4_example_platform.c`
- `stm32g4_beginner_demo.c`

Shared ADS1299 sources required by the current SDK core include the relevant files from:

- `firmware/core_driver/ads1299/`
- `firmware/common/data_packet/ads1299_packet.c`

Add include paths for:

- this `examples/` directory;
- `../ads1299_port/`;
- `../board/`;
- `firmware/core_driver/ads1299/`;
- `firmware/common/data_packet/`.

Use the repository's existing shared-core build metadata/file inventory as the authoritative source list; do not copy register logic into the STM32G4 folder.

## Expected bring-up output

Before the stream becomes binary canonical packets, the VCP log should progress approximately as:

```text
ADS1299 STM32G4 beginner demo
OK ID ADS1299-8
RUN internal-test
OK internal-test frames
RUN input-short
OK input-short frames
RUN EEG 250 SPS config
OK EEG 250 SPS configured
RUN stream canonical packets
```

ADS1299-4 and ADS1299-6 report their detected channel count instead. The canonical stream uses the repository packet encoder rather than a model-local format.

## Diagnostics

- `ERR RESET...`: inspect power, RESET and PWDN wiring/configuration.
- `ERR SDATAC...` or `ERR ID...`: inspect SPI Mode 1, CS, clock/power and MISO/MOSI wiring.
- `ERR DRDY timeout...`: inspect DRDY connection and ADS1299 conversion/start state.
- `ERR frame read...`: inspect SPI transfer/CS integrity.
- `ERR stream transport...`: inspect LPUART1/ST-LINK VCP configuration and host baud.

At 250 SPS, a 49-byte packet produces 12,250 payload bytes/s. 115200-baud 8-N-1 does not provide enough theoretical payload capacity for continuous full packet streaming; the reference starter uses 460800 baud.

## Validation claims

- Source integration: **PRESENT**.
- Progressive probe/internal-test/input-short/250-SPS/stream path: **PRESENT**.
- Shared canonical packet encoder use: **PRESENT**.
- Legacy debugger example: **PRESERVED**.
- STM32CubeIDE target build: **NOT YET RECORDED**.
- NUCLEO-G474RE + ADS1299 hardware run: **NOT BOARD-VERIFIED**.
- Sustained DMA/EXTI path: **NOT VERIFIED**.
- Multi-ADS1299 / 64-channel operation: **NOT VERIFIED**.
