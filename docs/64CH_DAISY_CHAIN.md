# 8 x ADS1299 / 64-channel scaling notes

This is an engineering roadmap for scaling eight ADS1299 devices to 64 channels. It is **not yet hardware-validated in this repository**.

## Two architectures

### A. Standard multi-device SPI

Keep separate chip-select/readback control per ADS1299 while sharing the bus where appropriate. This consumes more MCU/FPGA pins but makes per-device register configuration and debugging easier.

### B. ADS1299 daisy-chain mode

TI documents a daisy-chain mode selected through CONFIG1. In this mode SCLK, DIN and CS can be shared and one device's DOUT feeds the next device's DAISY_IN. Status and data from the device closest to the host appear first, followed by downstream device data.

Important TI constraints:

- CONFIG1 bit 6 is named `DAISY_EN`, but `0` selects daisy-chain mode and `1` selects multiple-readback mode.
- If all devices share DIN and therefore use the same register settings, TI states that an **external clock must be used** because the devices cannot be individually programmed through that shared-DIN arrangement.
- The BIAS driver is **not shared among multiple devices**.
- More devices make SCLK/DOUT-to-DAISY_IN setup/hold timing increasingly difficult; PCB topology and signal delay matter.
- TI recommends careful SCLK distribution and short DOUT paths and discusses buffering/delay options for difficult chains.

## Recommendation for the first 64-channel prototype

Do not optimize for minimum pin count first. Bring up two devices, then four, then eight. Keep enough independent control to isolate a faulty device during development. Only collapse to the most pin-efficient daisy topology after signal-integrity and synchronization tests pass.

## Required 64-channel validation

- common clock and START synchronization checked on an oscilloscope/logic analyzer
- all device IDs/configuration paths verified
- channel order proven with deterministic test signals
- 64-channel frame alignment tested under long runs
- packet-loss/frame-counter statistics recorded
- 250/500/1000 SPS throughput checked before higher rates
- PCB SCLK/DOUT integrity checked at the farthest device

Primary source: TI ADS1299 datasheet, Multiple Device Configuration / Daisy-Chain Mode: https://www.ti.com/lit/ds/symlink/ads1299.pdf
