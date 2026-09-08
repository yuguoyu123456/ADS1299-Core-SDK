# MIMXRT1170 + ADS1299 Ready-to-Run Guide

This guide only adds integration material; no existing files are removed or replaced.

## Minimal direct-use setup

- Reuse the existing NXP port and board support in this folder.
- Add `../_shared/nxp_ads1299_ready_app.c/.h` and the common ADS1299 core/profile sources.
- Configure LPSPI/SPI as master, SPI Mode 1 (CPOL=0, CPHA=1), MSB first.
- Use a conservative 2–4 MHz SPI clock for first validation.
- Configure CS, RESET, PWDN, START as outputs and DRDY as a falling-edge interrupt.

## Acquisition design

The DRDY interrupt must not perform transport, logging or signal processing. It should only notify a high-priority acquisition task/ISR-deferred handler. That context reads exactly one 27-byte ADS1299-8 frame, validates/stores it, increments the sample sequence and pushes it into a queue/ring buffer.

On dual-core RT1170 systems, place deterministic ADS1299 acquisition on one execution context and keep UI/network/storage work separated whenever practical.

## Validation order

1. Hardware reset and ID read.
2. `NXP_ADS1299_PROFILE_INTERNAL_TEST`.
3. `NXP_ADS1299_PROFILE_INPUT_SHORT`.
4. `NXP_ADS1299_PROFILE_EEG_250`.
5. Verify continuous 27-byte frames and the ADS1299 status header.
6. Add lead-off/BIAS only after the basic stream is stable.
7. Stress the transport path while monitoring queue-overflow and read-error counters.

## DMA/cache

For LPSPI DMA, use DMA-accessible memory and explicit cache maintenance where required by the RT1170 memory region selected for RX/TX buffers. Test cache-disabled/simple polling first if debugging unexplained frame corruption.

## 8 x ADS1299 / 64 channels

For 64-channel systems, the cleanest low-skew architecture is FPGA or parallel SPI/DMA capture feeding the RT1170 for control, buffering and communication. A single sequential SPI bus is acceptable only when the resulting inter-device skew is within the application requirement.

See `../DIRECT_USE.md` for the common NXP flow.