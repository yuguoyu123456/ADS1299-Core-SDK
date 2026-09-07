# LaunchPad reference binding

`launchpad_reference.c` is an original, cold-reset standalone binding for
MSPM0G3507. Do not call it after a bootloader or in an existing application
with another clock configuration: it assumes the reset SYSOSC source, sets
32 MHz and resets GPIOA/GPIOB/SPI1. For a product, supply a non-disruptive
board_ads1299_hal() using the adapter contract instead.

SPI1 uses PB9 SCK, PB8 PICO, PB7 POCI and PB6 software CS. Control signals
are PA25 DRDY, PA26 RESET, PA27 PWDN and PA28 START. The binding matches the
MCU's official IOMUX definitions; MCU pad names are not BoosterPack header
numbers. Check board revision, jumper conflicts and continuity before wiring.
See pinmap.md for electrical requirements. ADS1299 clock is supplied by its
own board; this example does not generate CLK. SPI is 1 MHz, Mode 1,
8-bit MSB-first. Hardware CS is not muxed. DRDY is polled.

The example writes the latest decoded frame to ads1299_latest_frame and
increments ads1299_frame_sequence twice per update. A stable even sequence
surrounding a debugger read gives a snapshot; older frames are overwritten.
This is an output demonstration, not a UART/USB recording implementation.
No successful hardware response is claimed. Use internal test input only
for initial bring-up; this is not a patient-connected medical device.
