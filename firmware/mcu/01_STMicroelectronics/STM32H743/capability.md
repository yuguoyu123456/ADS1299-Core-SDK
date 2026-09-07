# STM32H743 ADS1299 capability

| Item | Verified planning fact |
|---|---|
| CPU | Arm Cortex-M7 with DP-FPU/DSP/cache, silicon maximum 480 MHz |
| RAM | Up to 1 MiB total, including TCM and backup SRAM regions |
| Flash | 2 MiB dual-bank embedded Flash |
| SPI | Six SPI peripherals; SPI1 Mode 1 reference uses PA5/PA6/PA7 |
| DMA | MDMA plus DMA controllers; cache maintenance and DMA-visible memory are mandatory for a DMA design |
| USB | USB OTG FS and HS/FS capability |
| Ethernet | MAC with DMA; PHY and network stack remain outside the ADS1299 Port |
| BLE / Wi-Fi | No integrated radio on STM32H743; use an external module if required |

## Why this platform is valuable for ADS1299

The large RAM, deterministic Cortex-M7, multiple SPI blocks, DMA fabric, USB and
Ethernet make STM32H743 a strong acquisition controller for buffered EEG and
multi-device systems. The reference implementation intentionally starts with a
blocking 8-bit SPI transfer because ADS1299 frames are only 15/21/27 bytes and
this is easier to validate. DMA is an optimization stage, not an unverified
claim.

## Channel planning

- 8 channels / one ADS1299: directly supported by the shared Core call flow.
- 16 or 32 channels: practical with independent CS and observable DRDY lines;
  measure service latency and skew on the actual board.
- 64 channels: architecturally plausible, but requires DMA/queue scheduling,
  cache-safe buffers and measured throughput. It is not bench-validated here.
- EEG suitability: high for research acquisition after analog front-end, power,
  isolation and signal-integrity validation. The MCU cannot replace the ADS1299
  analog safety design.

Sources and revision details are recorded in `sources.md`.
