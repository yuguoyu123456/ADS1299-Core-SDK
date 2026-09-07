# STM32F407 ADS1299 capability

| Item | Verified planning fact |
|---|---|
| CPU | 168 MHz Arm Cortex-M4 with single-precision FPU and DSP instructions |
| RAM | Up to 192 KiB SRAM plus 4 KiB backup SRAM; 64 KiB CCM is not DMA-accessible |
| Flash | 1 MiB on STM32F407VGT6 |
| SPI | Three SPI peripherals; SPI1 reference is Mode 1 on PA5/PA6/PA7 |
| DMA | Two DMA controllers, 16 streams, FIFOs and burst support |
| USB | USB OTG FS plus HS/FS controller capability |
| Ethernet | 10/100 MAC with dedicated DMA; external PHY/stack required |
| BLE / Wi-Fi | No integrated radio |

## Why this platform is valuable for ADS1299

STM32F407 is a mature Cortex-M4 platform with deterministic SPI, DMA, memory and
USB/Ethernet options for EEG acquisition. On STM32F407G-DISC1, SPI1 is shared
with the LIS3DSH sensor, so PE3 must remain high while ADS1299 is selected. The
reference deliberately uses polling before any DMA claim.

- 8 channels: directly supported by the Core reference flow.
- 16/32 channels: feasible with per-device CS/DRDY and measured service latency.
- 64 channels: requires DMA-safe SRAM (not CCM), buffering and throughput tests;
  it is not hardware-validated here.
- EEG suitability: good for established research prototypes when the analog,
  isolation and power design is independently validated.

Sources and revision details are recorded in `sources.md`.
