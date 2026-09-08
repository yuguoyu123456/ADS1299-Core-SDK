# RA4M3 + ADS1299 Ready-to-Run Guide

This file is additive only. Existing RA4M3 project files remain unchanged.

## FSP integration

Reuse the current `ads1299_port/` and board abstraction. Bind the generic callbacks to Renesas FSP SPI/GPIO/external IRQ resources, then add `../_shared/renesas_ads1299_ready_app.c/.h` and the common ADS1299 core/profile sources.

## Peripheral setup

- SPI master, Mode 1 (CPOL=0, CPHA=1), MSB first.
- Begin at approximately 2–4 MHz during first hardware validation.
- CS, RESET, PWDN and START as outputs.
- DRDY as a falling-edge external IRQ input.

## Runtime architecture

Use the DRDY callback only to notify a high-priority acquisition context. That context reads one exact 27-byte ADS1299-8 frame, updates the sample/error counters and pushes the complete frame into a ring buffer.

Keep USB/UART/CAN/application processing downstream so communication latency cannot block sampling.

## Validation order

1. Hardware reset ADS1299.
2. Read and verify device ID.
3. Run `RENESAS_ADS1299_PROFILE_INTERNAL_TEST`.
4. Run `RENESAS_ADS1299_PROFILE_INPUT_SHORT`.
5. Run `RENESAS_ADS1299_PROFILE_EEG_250`.
6. Perform sustained captures while monitoring read errors and queue overflow.
7. Add BIAS, lead-off and faster rates only after the baseline stream is stable.

## DMA/DTC note

After a reliable polling/interrupt baseline is established, DMAC/DTC can be added. Use fixed ping-pong/ring buffers with explicit ownership between transfer completion and the consumer task.

## 64-channel note

For eight ADS1299 devices, prefer multiple independent SPI/DMA lanes or an FPGA capture stage for tighter synchronization. RA4M3 can then handle control, buffering and communication.

See `../DIRECT_USE.md` for the common Renesas integration sequence.