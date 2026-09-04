# Shared MCU reference application

This directory is the controller-neutral ADS1299 first-bring-up firmware used by MCU projects that have not yet graduated to a native high-performance port.

It performs real ADS1299 application logic: board initialization, hardware reset, device-ID read, 250 SPS configuration, gain 24, all-channel internal test source, RDATAC/START, DRDY wait, variant-aware frame acquisition and common packet streaming.

`generic_board_port.c` provides a conservative software SPI Mode 1 transport and software TX. A board project supplies `port/board_sdk.h` plus a vendor-SDK implementation of five primitive operations: init, pin write, pin read, microsecond delay and fatal handling.

A project is **not** `Compiles` merely because its profile exists. `Compiles` requires a clean build using its documented toolchain. After that, a performance revision should normally replace software SPI/TX with the MCU's native SPI/DMA/interrupt/USB/UART facilities where appropriate.
