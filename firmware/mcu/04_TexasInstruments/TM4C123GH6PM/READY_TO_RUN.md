# TM4C123GH6PM + ADS1299 Ready-to-Run Guide

This file adds practical integration guidance only. Existing TM4C123GH6PM files remain intact.

## TivaWare / DriverLib integration

Reuse the current `ads1299_port/` and board support in this folder. Bind the generic ADS1299 callbacks to SSI and GPIO using the project’s TivaWare/DriverLib layer, then add `../_shared/ti_ads1299_ready_app.c/.h` plus the common ADS1299 driver/profile sources.

## Peripheral setup

- SSI/SPI master, SPI Mode 1 (CPOL=0, CPHA=1), MSB first.
- Start bring-up around 2–4 MHz.
- CS/RESET/PWDN/START as outputs.
- DRDY as falling-edge GPIO interrupt.

## Runtime architecture

Keep the GPIO ISR minimal. It should only clear the interrupt and set a flag/semaphore. A high-priority acquisition context then performs one exact 27-byte ADS1299-8 frame transfer, validates/stores the frame and pushes it into a ring buffer.

UART/USB/CAN/logging code should consume buffered frames asynchronously and must not block the DRDY service path.

## Validation order

1. Hardware reset ADS1299.
2. Verify ID register.
3. `TI_ADS1299_PROFILE_INTERNAL_TEST`.
4. `TI_ADS1299_PROFILE_INPUT_SHORT`.
5. `TI_ADS1299_PROFILE_EEG_250`.
6. Run sustained capture while monitoring read errors and buffer overflow.
7. Add BIAS/lead-off and higher rates only after the baseline path is stable.

## DMA / uDMA note

Once a simple SSI transfer path is proven, uDMA can reduce CPU load. Use explicit RX buffer ownership and never let the consumer task overwrite a buffer that SSI/uDMA is still filling.

## 64-channel note

For eight ADS1299 devices, use FPGA or several independent SPI engines if tight device-to-device timing matters. TM4C123 can remain responsible for control and moderate-throughput transport.

See `../DIRECT_USE.md` for the common TI sequence.