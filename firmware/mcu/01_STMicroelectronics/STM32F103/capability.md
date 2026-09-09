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

## Current reference-device capability context

The current reproducible reference target is **NUCLEO-F103RB / STM32F103RBT6**. This section records the software design assumptions used by the model folder; it does not convert them into throughput or hardware-validation claims.

| Item | Current reference path |
|---|---|
| MCU | STM32F103RBT6 |
| Core class | Arm Cortex-M3 STM32F1 device class |
| Board | NUCLEO-F103RB (MB1136) |
| SDK/toolchain | STM32CubeMX + STM32CubeIDE / STM32CubeF1 HAL |
| ADS1299 SPI mode | Mode 1, 8-bit, MSB-first, software-controlled CS |
| Board config | `board/board_config.h` |
| Raw ADS1299-8 frame contract | 27 bytes |
| Canonical repository packet contract | 49 bytes |
| Starter bounded queue | 4 frames |
| Multi-device topology guidance | shared SPI with independent CS; not validated on this model |

The current beginner path is deliberately static/bounded: no heap allocation is required by the model-level board/port starter path, and transport work should remain outside the DRDY-critical path. DMA may be introduced as a platform optimization only after the basic polling/reference path is build- and board-verified; it must not duplicate shared ADS1299 register behavior.

### Validation limits

- Reference integration files present: **yes**
- Host adapter/integration test infrastructure present: **yes**
- STM32CubeIDE BUILD-VERIFIED: **no**
- NUCLEO-F103RB + ADS1299 BOARD-VERIFIED: **no**
- measured maximum SPI rate: **not claimed**
- measured sustained transport rate: **not claimed**
- multi-ADS1299 count/timing: **not claimed**

Treat all capacity beyond a single-device starter integration as an engineering target until measured with the exact clock tree, HAL configuration, transport and ADS1299 hardware.
