# ESP32-DevKitC V4 / WROOM-32 reference

This profile is for a 4 MB WROOM module, not a claim covering WROVER/PSRAM or
every board sold as ESP32. Header positions follow the official DevKitC V4 guide.

| ADS1299 signal | GPIO | Header |
| --- | --- | --- |
| SCLK | 18 | J3-9 |
| DIN / MOSI | 23 | J3-2 |
| DOUT / MISO | 19 | J3-8 |
| CS | 21 | J3-6 |
| RESET | 22 | J3-3 |
| PWDN | 25 | J2-9 |
| START | 26 | J2-10 |
| DRDY | 27 | J2-11 |
| Ground | GND | J2-14 / J3-1 |

SPI3/VSPI, 1 MHz, Mode 1, MSB-first, 8-bit, no DMA, queue size 1. CS is GPIO;
only app_main may own the bus, including between 32-byte chunks. GPIO6–11 are
reserved for module Flash and not used. Boot strapping and UART0 pins remain
unchanged. The adapter rejects input-only GPIO34–39 for output signals.

Use compatible 3.3 V ADS1299 DVDD and common ground; no 5 V signals. Confirm your
physical board/module before wiring. No unisolated person-connected electrodes.
No ADS1299 CLK signal is generated; configure the chip clock as documented.

The persistent descriptor/buffers are not freed after timeout. Do not remove the
SPI device, reinitialize context or retry until driver ownership has been resolved.
There is no automatic recovery. A different task must not access this context.

The example waits one 1 ms RTOS tick each loop, then checks DRDY. A debugger can
read ads1299_latest_frame while observing the odd/even sequence convention, but
this is not lossless EEG transport. Radio coexistence, sampling latency, dropped
DRDY edges and actual SPI/CS waveform timing remain untested on hardware.
