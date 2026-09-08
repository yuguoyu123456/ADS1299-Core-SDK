# CC32xx + ADS1299 Ready-to-Run Guide

This guide is additive only. Existing CC32xx support remains unchanged.

## Integration path

Keep the current `ads1299_port/` and board files. Connect the generic ADS1299 callbacks to the SimpleLink SPI/GPIO implementation used by the CC32xx project, then add the shared TI helper and the common ADS1299 core/profile sources.

## SPI and DRDY

- SPI master, Mode 1 (CPOL=0, CPHA=1), MSB first.
- Start around 2–4 MHz for bring-up.
- CS/RESET/PWDN/START as outputs.
- DRDY falling-edge interrupt input.

## Acquisition versus Wi-Fi

Recommended pipeline:

`DRDY -> high-priority acquisition task -> exact 27-byte ADS1299 frame -> ring buffer -> Wi-Fi/TCP/UDP/application task`

Do not let socket operations, WLAN callbacks, DNS/TLS work or printf block the acquisition task. If the network stalls, preserve acquisition timing and expose queue-overflow counters.

## Validation order

1. Hardware reset and ID verification.
2. `TI_ADS1299_PROFILE_INTERNAL_TEST`.
3. `TI_ADS1299_PROFILE_INPUT_SHORT`.
4. `TI_ADS1299_PROFILE_EEG_250`.
5. Long continuous capture while Wi-Fi is idle.
6. Repeat while Wi-Fi traffic is active and confirm frame alignment/read-error/overflow counters remain acceptable.
7. Only then add BIAS, lead-off and faster data rates.

## 64-channel note

For 8 x ADS1299, deterministic capture should preferably be handled by FPGA or multiple dedicated SPI/DMA paths. CC32xx can then provide Wi-Fi control and uplink without coupling RF/network latency into the sample clock path.

See `../DIRECT_USE.md` for the common TI flow.