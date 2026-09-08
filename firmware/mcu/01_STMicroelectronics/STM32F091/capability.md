# Capability

| Item | Capability |
|---|---|
| CPU | confirm against selected device |
| RAM | confirm against selected device |
| Flash | confirm against selected device |
| SPI | Planned: SPI Mode 1, 8-bit MSB-first; frequency requires board validation |
| DMA | confirm against selected device |
| USB | confirm against selected device |
| BLE | confirm against selected device |
| Multi-ADS1299 | Not validated; first establish one ADS1299, then measure buffering and timing |

These are planning limits, not throughput evidence. Bench measurements decide
the final supported ADS1299 count and transport rate.

---

## Current reference-device capability: STM32F091RCT6

The planning table above is retained because other STM32F091 variants may differ. For the current NUCLEO-F091RC reference path, the selected STM32F091RCT6 belongs to the STM32F091xC family and provides the following documented device-level resources:

| Item | Current reference capability |
|---|---|
| CPU | Arm Cortex-M0, up to 48 MHz |
| Flash | 256 KiB for the STM32F091RC xC density device |
| SRAM | 32 KiB |
| SPI | 2 SPI peripherals, documented up to 18 Mbit/s; ADS1299 integration still uses Mode 1, 8-bit, MSB-first and a conservative bring-up clock |
| DMA | 12-channel DMA controller |
| USART | up to 8 USARTs on the Rx device family |
| CAN | 1 CAN interface |
| USB device peripheral | Not listed as an integrated STM32F091xB/xC communication peripheral; do not assume native USB for this reference path |
| BLE | No integrated BLE radio; external radio/module required |
| Multi-ADS1299 | Software architecture can use shared SPI plus independent CS, but device count, buffering, DRDY latency and transport throughput remain unvalidated on this MCU |

The NUCLEO-F091RC reference integration deliberately starts with one ADS1299. Multi-device claims require measured queue occupancy, SPI service time, DRDY handling and transport-loss accounting before status promotion.

## ADS1299 data-rate implications

For the repository beginner configuration, one ADS1299-8 produces one 27-byte raw frame per sample. The canonical repository stream packet is 49 bytes. At 250 SPS, that is 12,250 packet payload bytes/s before UART framing for one ADS1299 stream. The simple blocking-UART example is for bring-up only; sustained or multi-device acquisition should use bounded buffering and interrupt/DMA-assisted transport so host output cannot stall DRDY servicing.

These capability statements are device/document limits and software architecture notes, not board validation or throughput evidence.
