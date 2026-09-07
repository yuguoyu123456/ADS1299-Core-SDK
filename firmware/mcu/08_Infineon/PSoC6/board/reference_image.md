# CM4 cold-start reference profile

Target: CY8C624ABZI-S2D44, CY8CKIT-062S2-43012 identity. This is an original MCU
signal profile, not a verified board-header wiring guide. Confirm board revision,
on-board loads and physical accessibility before connecting anything.

| ADS1299 signal | MCU signal | Owner |
| --- | --- | --- |
| DIN / MOSI | P6.0 | SCB3 MOSI |
| DOUT / MISO | P6.1 | SCB3 MISO |
| SCLK | P6.2 | SCB3 CLK |
| CS | P6.3 | GPIO, initial high |
| RESET | P7.0 | GPIO, initial high |
| PWDN | P7.1 | GPIO, initial high |
| START | P7.2 | GPIO, initial low |
| DRDY | P7.3 | Input, active low |

SCB alternate functions come from the official exact-device headers. Header
numbers and electrical routing are unverified. Match the MCU I/O supply to
ADS1299 DVDD, verify voltage limits in both official datasheets, and share ground.
Do not connect electrodes to people during an unisolated development setup.

The reference owns SCB3 and peripheral 16-bit divider 0 exclusively on CM4.
It inherits the official CM0P_SLEEP/BSP startup clocks, calls
SystemCoreClockUpdate to refresh delay calibration, then divides peripheral Hz
down to at most 8 MHz; oversample 8 gives SCLK at most 1 MHz, Mode 1, MSB first,
8-bit words. Do not change clocks, sleep, enable SCB IRQ/DMA, or share this SCB
with HAL/CM0+ while collecting. Delay calls split into <=60000 us chunks to avoid
the PDL uint16_t argument truncating longer Core delays.

No ADS1299 CLK output is generated. Use the chip's documented internal/external
clock configuration. IRQ is disabled; DRDY is polled. The application exposes a
latest-frame debugger snapshot only; this is not a lossless EEG transport.

The build consumes unchanged external startup/linker resources and the official
prebuilt CM0+ image. Do not add a second board_ads1299_hal implementation.
Software link/model evidence is not proof of physical cold boot or acquisition.
