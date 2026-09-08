# STM32C031 integration with STM32CubeC0

Primary reference board: **NUCLEO-C031C6 (MB1717, STM32C031C6T6)**.

Validation state: **TEMPLATE / integration-complete; not BUILD-VERIFIED or BOARD-VERIFIED**.

## 1. Create the vendor-owned project

Install STM32CubeIDE with STM32CubeC0 support, then create a new project targeting `NUCLEO-C031C6` or `STM32C031C6T6`.

Keep ST-owned startup code, linker scripts, CMSIS, HAL, clock configuration and generated peripheral initialization in that Cube project. Do not copy vendor SDK files into this repository.

## 2. Configure peripherals in CubeMX

Configure one SPI peripheral with:

- master mode;
- full duplex;
- CPOL = Low;
- CPHA = 2 Edge (**SPI Mode 1**);
- MSB first;
- software NSS/CS;
- conservative prescaler for first bring-up.

Create GPIO labels:

- `ADS1299_CS`
- `ADS1299_DRDY`
- `ADS1299_RESET`
- `ADS1299_PWDN`
- `ADS1299_START`

Configure DRDY as an input. Configure the four control signals as outputs with startup states appropriate to the ADS1299 bring-up sequence. If using the included output path, enable a UART and expose its HAL handle through `board/board_config.h`.

## 3. Add repository-owned sources

Add the shared ADS1299 core sources from:

`firmware/core_driver/ads1299/`

Add the STM32C031 adapter sources from:

`firmware/mcu/01_STMicroelectronics/STM32C031/ads1299_port/`

Add the example-owned sources:

- `examples/stm32c031_example_platform.c`
- `examples/stm32c031_beginner_demo.c`

Use `examples/main_ads1299.c` only when you specifically want the older compatibility-style entry point.

If using canonical 49-byte streaming, also add the repository packet implementation from:

`firmware/common/data_packet/`

Add include paths for the shared core, `board/`, `ads1299_port/`, `examples/`, and packet folder when used.

## 4. Edit only the board/config layer

Edit `board/board_config.h` to bind the Cube-generated SPI handle, UART handle if used, and GPIO labels/ports. A normal new-board port should not require edits to `ads1299.c`, `ads1299_regs.h`, `ads1299_model.c`, or other shared ADS1299 files.

For another board using the same STM32C031 MCU, keep the shared core and examples unchanged and replace only these board/peripheral bindings where practical.

## 5. Call the beginner demo

After the Cube-generated sequence has run:

```c
HAL_Init();
SystemClock_Config();
MX_GPIO_Init();
MX_SPIx_Init();
MX_USARTx_UART_Init();
```

include `stm32c031_beginner_demo.h` and call the documented demo function from the user section of `main.c`. See `examples/README.md` for the exact current entry-point signature and expected staged behavior.

The beginner flow performs the logical progression:

1. platform/port initialization;
2. ADS1299 reset and command-mode entry;
3. device ID probe;
4. internal-test configuration/readout;
5. input-short configuration/readout;
6. 250-SPS EEG configuration;
7. continuous frame acquisition;
8. optional canonical packet output;
9. clean stop.

## 6. Callback semantics

The shared port `spi_transfer` callback is full duplex. A null TX pointer means clock zero bytes while receiving; a null RX pointer means discard received bytes. If a HAL path cannot directly represent null buffers, the STM32 adapter must implement this behavior with bounded local/static scratch storage rather than pushing this concern into the shared ADS1299 core.

CS is controlled by the separate GPIO callback and must not be secretly toggled inside `spi_transfer`.

DRDY is timing-sensitive. Do not block on UART/USB/network output inside a DRDY ISR. For this small MCU, prefer bounded static buffering and raw/integer samples in the timing-critical path.

## 7. First validation sequence

Do not connect electrodes as the first test. Validate in this order:

1. power rails, reference and ADS1299 clock;
2. RESET/PWDN/START logic;
3. SPI Mode 1 and device ID;
4. register/configuration readback where used;
5. internal test source;
6. input-short baseline;
7. DRDY-driven 250-SPS acquisition;
8. transport/packet streaming.

If device ID fails, do not compensate by editing shared register definitions. Fix power, wiring, SPI mode, CS, board handle selection, or reset sequencing first.

## SDK ownership boundary

Vendor startup, linker scripts, CMSIS/HAL, generated configuration and middleware stay in the consuming official Cube project. This repository owns the ADS1299 shared core, the thin STM32C031 adaptation, examples, packet helpers and repository-side software tests.

Pin the STM32CubeC0 package version in the consuming project when reproducibility matters and record material integration changes in `version.md`.

## Validation boundary

This integration recipe is repository-owned source guidance. It is **not** evidence that a current STM32CubeIDE build has been executed, nor evidence of a physical NUCLEO-C031C6 + ADS1299 test. Upgrade the status to BUILD-VERIFIED or BOARD-VERIFIED only when that evidence exists.
