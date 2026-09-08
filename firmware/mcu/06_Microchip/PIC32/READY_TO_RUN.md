# PIC32 + ADS1299 Ready-to-Run Guide

This guide is additive only. Existing PIC32 files remain unchanged.

## Harmony 3 integration

Reuse the current `ads1299_port/` and board support in this folder. Bind the generic callbacks to Harmony 3 SPI PLIB/driver, GPIO and external interrupt resources selected by the target device, then add `../_shared/microchip_ads1299_ready_app.c/.h` plus the common ADS1299 core/profile sources.

## Peripheral setup

- SPI master, Mode 1 (CPOL=0, CPHA=1), MSB first.
- Start around 2–4 MHz during first hardware validation.
- CS, RESET, PWDN and START as outputs.
- DRDY as a falling-edge external interrupt input.

## Acquisition architecture

Use the DRDY ISR/callback only to signal a high-priority acquisition context. That context reads one exact 27-byte ADS1299-8 frame, updates sequence/error counters and queues the complete frame.

Recommended flow:

`DRDY -> short ISR -> SPI frame read -> ring buffer -> USB/UART/Ethernet/CAN/application`

Do not let communication stacks or signal processing block the sample path.

## Validation sequence

1. Hardware reset ADS1299.
2. Verify device ID.
3. `MICROCHIP_ADS1299_PROFILE_INTERNAL_TEST`.
4. `MICROCHIP_ADS1299_PROFILE_INPUT_SHORT`.
5. `MICROCHIP_ADS1299_PROFILE_EEG_250`.
6. Long continuous capture with frame/header checks.
7. Stress the final transport while monitoring read errors and queue overflow.
8. Add BIAS, lead-off and higher sample rates only after baseline stability.

## DMA/cache note

After proving a simple polling/interrupt SPI path, enable DMA where supported with explicit ping-pong/ring-buffer ownership. On cached PIC32 variants, place DMA buffers in appropriate memory and perform required cache maintenance.

## 64-channel note

For eight ADS1299 devices, multiple independent SPI/DMA lanes or an FPGA capture front-end are preferred when tight synchronization matters. PIC32 can then handle aggregation, DSP and transport.

See `../DIRECT_USE.md` for the common Microchip integration flow.