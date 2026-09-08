# CC26xx + ADS1299 Ready-to-Run Guide

This guide is additive only. Existing CC26xx files remain untouched.

## Integration

Reuse the current `ads1299_port/` and board abstraction. Connect the generic ADS1299 SPI/GPIO callbacks to the SimpleLink / TI Drivers implementation already used by the target project, then add the shared TI ready-app helper and ADS1299 core/profile sources.

## SPI / GPIO

- SPI master, Mode 1 (CPOL=0, CPHA=1), MSB first.
- Bring up at approximately 2–4 MHz before optimizing.
- CS, RESET, PWDN and START are outputs.
- DRDY is a falling-edge interrupt input.

## Real-time data path

Recommended flow:

`DRDY -> event/semaphore -> high-priority acquisition task -> one complete 27-byte frame -> ring buffer -> BLE/serial/radio task`

Do not put radio-stack work, printf, filesystem access or signal processing in the DRDY callback.

## Validation sequence

1. ADS1299 hardware reset.
2. Verify device ID.
3. Run `TI_ADS1299_PROFILE_INTERNAL_TEST`.
4. Run `TI_ADS1299_PROFILE_INPUT_SHORT`.
5. Run `TI_ADS1299_PROFILE_EEG_250`.
6. Perform a long continuous capture and watch frame alignment/read-error/queue-overflow counters.
7. Enable BIAS/lead-off and higher sample rates only after stable baseline capture.

## Wireless note

Connection events and RF stack scheduling can introduce latency. Keep sampling and transport separated with a bounded queue so delayed BLE/radio service cannot stall ADS1299 acquisition.

## 64-channel note

For eight ADS1299 devices, use FPGA or multiple hardware SPI acquisition paths if tight cross-device timing matters. CC26xx devices are best used as control/wireless endpoints when the 64-channel capture stage is separated.

See `../DIRECT_USE.md` for the shared TI path.