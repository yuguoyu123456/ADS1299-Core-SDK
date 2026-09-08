# nRF52833 + ADS1299 Ready-to-Run Guide

This file is additive only. Existing nRF52833 support remains intact.

## SDK integration

Reuse this directory's existing `ads1299_port/` and board abstraction. Bind the generic callbacks to nRF Connect SDK/Zephyr SPI/GPIO APIs or the legacy nRF5 SDK SPIM/GPIOTE layer used by the project, then add the shared Nordic ready-app and common ADS1299 core/profile sources.

## Peripheral setup

- SPIM/SPI master, Mode 1 (CPOL=0, CPHA=1), MSB first.
- Begin around 2–4 MHz during first validation.
- CS, RESET, PWDN and START as outputs.
- DRDY as a falling-edge GPIO/GPIOTE interrupt input.

## Real-time flow

Use the DRDY callback only to signal a high-priority acquisition context. That context reads one exact 27-byte ADS1299-8 frame, updates sequence/error counters and stores the frame into a ring buffer.

Keep BLE/802.15.4/USB/application work downstream so radio or protocol scheduling cannot block sampling.

## Validation sequence

1. Hardware reset ADS1299.
2. Verify device ID.
3. Run `NORDIC_ADS1299_PROFILE_INTERNAL_TEST`.
4. Run `NORDIC_ADS1299_PROFILE_INPUT_SHORT`.
5. Run `NORDIC_ADS1299_PROFILE_EEG_250`.
6. Perform long capture with radio idle and again with representative wireless traffic.
7. Monitor frame alignment, read errors and queue overflow.
8. Add BIAS, lead-off and higher rates only after stable baseline operation.

## EasyDMA / PPI note

After a simple SPIM transfer path is proven, EasyDMA can reduce CPU occupancy. GPIOTE/PPI can also reduce DRDY-to-transfer trigger jitter where the selected SDK/peripheral configuration supports it, but validate that optimization separately from the base path.

## 64-channel note

For eight ADS1299 devices, multiple deterministic SPI/DMA lanes or FPGA capture are preferred when cross-device timing matters. nRF52833 can then focus on wireless control and transport.

See `../DIRECT_USE.md` for the common Nordic integration flow.