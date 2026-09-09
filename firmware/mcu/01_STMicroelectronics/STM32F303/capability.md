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

## Current reference-device capability: STM32F303RET6

The following facts apply to the selected NUCLEO-F303RE reference MCU and replace the earlier unknown planning placeholders for the current reference path:

| Item | Current reference capability |
|---|---|
| CPU | Arm Cortex-M4 with FPU, up to 72 MHz |
| Flash | 512 KB |
| SRAM | 64 KB SRAM + 16 KB CCM SRAM (80 KB total memory class) |
| DMA | 12-channel DMA controller |
| SPI | STM32F303RET6 provides multiple SPI interfaces; repository reference path uses SPI1, Mode 1, 8-bit, MSB-first |
| USB | Device family includes USB; not part of the current beginner ADS1299 transport path |
| BLE | No on-chip BLE radio; external radio would be a separate transport integration |
| Starter buffering | Board config defines a bounded static 8-frame starter queue |
| ADS1299 frame | 27-byte raw frame for ADS1299-8 continuous data |
| Repository packet | 49-byte canonical packet where the beginner example uses packet streaming |
| Multi-ADS1299 | Architecture may be extended using shared SPI plus independent CS, but STM32F303 multi-device timing/throughput is not yet validated |

These are device/resource facts and repository design limits, not evidence of sustained ADS1299 throughput, DMA correctness, board validation, or 64-channel performance. Those require measured validation.