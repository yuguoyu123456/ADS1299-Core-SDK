# FSP 5.6 reference image profile

`reference_image.c` supplies an original bare-metal board binding, four SPI1
interrupt vectors/event links and a polled DWT microsecond timebase. Build it
instead of `fsp_binding.c`; do not combine the two board bindings. Only the
external TinyUSB 0.18.0 BSP/clock configuration is consumed; no USB stack is used.

| ADS1299 signal | MCU signal | Official example board label |
|---|---|---|
| SCK | P412 / SPI1 | PMOD2_SCK |
| DIN | P411 / SPI1 | PMOD2_MOSI |
| DOUT | P410 / SPI1 | PMOD2_MISO |
| CS | P408 / GPIO | PMOD2_SS2 |
| DRDY | P400 / GPIO input | PMOD2_INT |
| RESET | P404 / GPIO | PMOD2_RST |
| PWDN | P708 / GPIO | PMOD2_SS3 |
| START | P112 / GPIO | ARDUINO_D4 |

These signal labels come from the official EK-RA6M5 example configuration cited
in ../sources.md. They are **not verified connector-number assignments**. Our
assignment of those GPIO signals to ADS1299 control is an original reference
choice, not a Renesas ADS1299 example. Remove attached PMOD/Arduino modules and
verify board revision routing/solder bridges before wiring. Use compatible 3.3 V
digital supplies and a common digital ground. No patient-connected testing is
covered by this build.

SPI1 uses Mode 1, 8-bit MSB transfers, 1 MHz calculated by FSP, clock-synchronous
3-wire peripheral mode plus independent GPIO CS. SPI interrupts occupy vector
slots 0..3 at priority 5; do not merge this table unchanged into another firmware.
CS/RESET/PWDN are configured output-high, START output-low before SPI opens.

DWT clock requires the counter to be available and advancing; initialization
checks this. The accumulator must be polled at least once per counter wrap
(approximately 21 seconds at 200 MHz). The example polls continuously, and long
delays service it every 100 ms. Do not change clocks, sleep, or treat debug-halt
time as wall time. A fixed poll failsafe prevents a stalled timebase hanging a
SPI transaction, but is not a calibrated acquisition deadline.

The FSP startup/ROM option settings and TinyUSB clock configuration are not copied
or changed here. This is a cold-start flat-image reference, not a bootloader,
TrustZone deployment or ready-to-flash board qualification. Review option bytes
and debugger programming settings before flashing. No flash operation was run.
