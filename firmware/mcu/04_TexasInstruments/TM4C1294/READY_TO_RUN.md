# TM4C1294 + ADS1299 Ready-to-Run Guide

This guide is additive only. Existing TM4C1294 files remain unchanged.

## TivaWare / DriverLib integration

Reuse the current `ads1299_port/` and board support in this directory. Bind the generic ADS1299 callbacks to the selected SSI and GPIO resources, then add the shared TI ready-app helper and common ADS1299 core/profile sources.

## Peripheral setup

- SSI/SPI master, Mode 1 (CPOL=0, CPHA=1), MSB first.
- Use a conservative 2–4 MHz clock during first validation.
- CS, RESET, PWDN and START as outputs.
- DRDY as a falling-edge GPIO interrupt.

## Acquisition pipeline

`DRDY -> short GPIO ISR -> acquisition task/deferred handler -> exact 27-byte ADS1299-8 frame -> ring buffer -> Ethernet/USB/UART/application`

Keep Ethernet/TCP/IP, USB and logging outside the sampling ISR. If transport slows, acquisition should continue until the ring buffer fills and the overflow counter should make the loss visible.

## Validation order

1. Hardware reset ADS1299.
2. Verify the ID register.
3. Run `TI_ADS1299_PROFILE_INTERNAL_TEST`.
4. Run `TI_ADS1299_PROFILE_INPUT_SHORT`.
5. Run `TI_ADS1299_PROFILE_EEG_250`.
6. Perform long captures with transport idle and then under heavy Ethernet/USB traffic.
7. Add BIAS, lead-off and higher rates only after stable baseline capture.

## uDMA / Ethernet note

TM4C1294 is a good fit for buffered Ethernet transport, but network servicing must remain downstream of acquisition. After a polling/interrupt SPI path is proven, SSI uDMA can be added with explicit ping-pong/ring-buffer ownership.

## 64-channel note

For 8 x ADS1299, use multiple SSI/DMA lanes or an FPGA front-end when tight synchronization is required. TM4C1294 can then handle packetization, Ethernet and system control.

See `../DIRECT_USE.md` for the common TI flow.