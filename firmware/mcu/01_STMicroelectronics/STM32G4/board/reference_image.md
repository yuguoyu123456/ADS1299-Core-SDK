# Original STM32G474RE cold-reset reference

NUCLEO-G474RE identity; **physical header mapping remains unverified**.
SPI GPIO signals follow the official CubeG4 NUCLEO polling-master example.
Control GPIOs are original provisional choices, not claims about connector pins.

| ADS1299 signal | MCU signal | Setup |
| --- | --- | --- |
| SCLK | PB3 | SPI1 AF5 |
| DOUT / MISO | PB4 | SPI1 AF5 |
| DIN / MOSI | PB5 | SPI1 AF5 |
| CS | PC0 | GPIO high |
| RESET | PC1 | GPIO high |
| PWDN | PC2 | GPIO high |
| START | PC3 | GPIO low |
| DRDY | PC4 | Input, active low |

Use SWD only: PB3/PB4 must not be owned by SWO/JTAG. Check solder bridges,
on-board loads and board revision before wiring. Match I/O supply to ADS1299 DVDD,
verify both device voltage limits and connect common ground. No people-connected
electrodes on an unisolated development setup.

Only reset HSI16, undivided AHB/APB2 is accepted. SPI1 is reset exclusively,
configured to Mode 1, MSB-first, 8-bit with byte FIFO threshold and /16 baud.
No PLL change, HAL SPI, DMA or SPI IRQ is used. DRDY is polled. DWT supplies the
16-cycle/us delay with a finite stalled-counter guard; do not change clocks or
enter sleep while acquiring. On a delay fault, following transfers fail.

No ADS1299 clock output is supplied. Select internal/external clock according to
the chip documentation. The application stores only a latest-frame debugger
snapshot, not a continuous lossless EEG stream. Pin readiness and actual clock
waveforms must be measured on hardware before marking Bench-tested.
