# CC1352 + ADS1299 Ready-to-Run Guide

This is an additive guide only. Existing CC1352 files remain unchanged.

## Integration path

Reuse this folder’s current `ads1299_port/` and board abstraction, then add the shared TI ready-app helper and ADS1299 core/profile sources.

Configure the selected SPI peripheral as master, SPI Mode 1 (CPOL=0, CPHA=1), MSB first. Start with a conservative 2–4 MHz clock during hardware bring-up.

## DRDY handling

Configure DRDY for a falling-edge GPIO interrupt. The interrupt/callback should only signal a high-priority acquisition context. That context performs exactly one 27-byte ADS1299-8 transfer, checks the frame/status, increments the sample sequence and queues the complete frame.

Do not perform BLE/Sub-1 GHz stack work in the DRDY callback.

## Validation

- Reset ADS1299 and verify ID.
- `TI_ADS1299_PROFILE_INTERNAL_TEST`.
- `TI_ADS1299_PROFILE_INPUT_SHORT`.
- `TI_ADS1299_PROFILE_EEG_250`.
- Confirm sustained frame alignment with no queue overflow.
- Add BIAS, lead-off and faster sample rates only after the baseline path is stable.

## Wireless scheduling

CC1352 can combine multiple wireless paths, so isolate the sampling task from RF scheduling. Use a bounded ring buffer between acquisition and radio packetization, and expose overflow counters rather than silently dropping samples.

## 64-channel architecture

For 8 x ADS1299, use FPGA or multiple deterministic SPI lanes when inter-device skew matters. CC1352 is better positioned as the control/wireless endpoint after capture is made deterministic.

See `../DIRECT_USE.md` for the common TI integration sequence.