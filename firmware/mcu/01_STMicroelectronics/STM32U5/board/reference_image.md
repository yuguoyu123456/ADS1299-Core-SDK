# U575 reset reference, not qualified board wiring

NUCLEO-U575ZI-Q, STM32U575ZI; **TrustZoneDisabled only**. Do not use this as a
nonsecure payload under a secure loader. Security option bytes are never changed;
the FLASH option read is a consistency check, not a security-attribution setup.

| ADS1299 signal | MCU signal | Setup |
| --- | --- | --- |
| SCLK | PA5 | SPI1 AF5 |
| DOUT / MISO | PA6 | SPI1 AF5 |
| DIN / MOSI | PA7 | SPI1 AF5 |
| CS | PC0 | GPIO initially high |
| RESET | PC1 | GPIO initially high |
| PWDN | PC2 | GPIO initially high |
| START | PC3 | GPIO initially low |
| DRDY | PC4 | Input active low |

SPI MCU signals follow the official polling example; controls are original
provisional choices. Header numbers, bridges and on-board loads are unverified.
Confirm board revision and voltage compatibility with ADS1299 DVDD before wiring,
with common ground. No person-connected electrodes on an unisolated development rig.

The reference accepts only reset MSI4 and undivided AHB, selects SYSCLK for SPI1,
and uses /4 for nominal 1 MHz, Mode 1, MSB-first, 8-bit. Owns SPI1 exclusively:
no HAL, DMA, IRQ, autonomous trigger or power/clock changes while acquiring.
DWT supplies 4 cycles/us delay, with a stalled-counter guard and sticky fault.
No ADS1299 clock output is supplied. Output is only a latest-frame debugger snapshot.

ES0499 warns about disabling SPI too soon after EOT. The adapter waits two actual
SCK periods rounded upward to whole microseconds before disabling. This is a
conservative implementation choice, not an ST-specified numerical guarantee;
scope the actual last edge and CS timing on every hardware/clock profile.
The delay callback must satisfy minimum timing and set context.fault on failure.
