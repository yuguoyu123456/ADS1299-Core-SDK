# ESP32-C6-DevKitC-1 v1.2 reference

SPI2, 1 MHz, Mode 1, 8-bit MSB-first, DMA disabled, queue size 1, software CS.
GPIO routing is checked against the official v1.2 user-guide header tables;
electrical behavior has not been bench-tested. Not the DevKitM-1 or v1.1 pinout.

| ADS1299 signal | GPIO | Header |
| --- | --- | --- |
| SCLK | 6 | J1-5 |
| DIN / MOSI | 7 | J1-6 |
| DOUT / MISO | 2 | J1-12 |
| CS | 10 | J1-10 |
| DRDY | 3 | J1-13 |
| RESET | 18 | J3-10 |
| PWDN | 19 | J3-9 |
| START | 1 | J1-8 |
| Ground | GND | J1-15 / J3-1 |

RESET/PWDN deliberately no longer use GPIO4/5: those are boot strapping pins.
Do not connect using an older generated pin map. GPIO6/7 must not be used by
external pin JTAG while they belong to SPI; USB Serial/JTAG pins 12/13 remain free.
GPIO1 must not be used by an external 32 kHz crystal in this reference.
Use compatible 3.3 V ADS1299 DVDD and common ground; no 5 V signals. Confirm board
revision and module supply before wiring. No unisolated person-connected electrodes.

Only app_main owns this bus/context. Do not add another SPI device/task, DMA,
radio-dependent acquisition scheduling, or asynchronous use of Core callbacks.
Long transfers use 32-byte internal segments without changing GPIO CS. On timeout
the persistent transaction/buffers remain reserved for the driver; do not free,
reinitialize or remove them. Automatic recovery is not provided.

The minimal example yields one 1 ms FreeRTOS tick per acquisition-loop iteration
so idle tasks can run. Its latest-frame debugger snapshot is not lossless streaming.
Real timing, missed DRDY edges and radio coexistence require separate measurement.
No ADS1299 CLK output is generated; use its documented clock configuration.
