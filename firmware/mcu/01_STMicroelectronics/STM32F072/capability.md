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

## Current reference-device capability

For the current first-path target **STM32F072RBT6 on NUCLEO-F072RB**, ST documents the following device capabilities:

| Item | STM32F072RBT6 reference capability | SDK policy |
|---|---|---|
| CPU | Arm Cortex-M0, up to 48 MHz | Keep timing-critical acquisition simple and bounded. |
| Flash | 128 KiB | Shared ADS1299 core plus model integration must remain compact. |
| SRAM | 16 KiB | Prefer static frame/packet buffers; avoid large dynamic queues. |
| SPI | Two SPI peripherals, device capability up to 18 Mbit/s | Beginner ADS1299 path uses SPI Mode 1, 8-bit, MSB-first and a conservative clock; actual board signal integrity is not yet validated. |
| DMA | Seven-channel DMA controller | Appropriate future path for SPI/UART isolation, but not claimed as implemented/bench-verified by the current beginner demo. |
| USB | USB 2.0 full-speed device capability | Available as a future transport option; current beginner integration demonstrates UART transport instead of inventing a USB stack. |
| BLE | No integrated BLE radio | Use an external radio/module or a different wireless MCU; never block DRDY servicing on radio work. |
| Multi-ADS1299 | Shared core supports multi-device architecture | No STM32F072 multi-device throughput count is claimed until buffering/latency measurements exist. |

### Buffering and transport guidance

An ADS1299-8 raw frame is 27 bytes and the repository canonical packet is 49 bytes. The STM32F072 integration therefore favors small static buffers and bounded queues. At 250 SPS, the canonical packet payload rate is 12,250 bytes/s before UART framing, so the blocking reference stream is an instructional path rather than a production-grade real-time transport design.

For a production-oriented STM32F072 implementation, the next technical improvement should be interrupt/DMA-driven acquisition and transport with explicit queue overflow counters. Do not silently drop frames and do not perform long UART/USB work in the DRDY timing path.

### Validation boundary

The device capabilities above come from ST's STM32F072RB documentation; they do not prove this repository's achieved throughput. Current repository status remains **TEMPLATE / integration candidate-complete**, not BUILD-VERIFIED or BOARD-VERIFIED.
