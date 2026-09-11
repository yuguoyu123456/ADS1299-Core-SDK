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

## Current implemented integration capabilities

The historical planning table above is retained. The current STM32H563 model folder additionally has the following repository capabilities:

| Capability | Current state |
|---|---|
| Reference board path | PRESENT: NUCLEO-H563ZI / STM32H563ZIT6 |
| Single board/config edit point | PRESENT: `board/board_config.h` |
| Portable ADS1299 port | PRESENT: SPI/GPIO/DRDY adapter layer |
| Probe / ID path | PRESENT in progressive beginner example |
| Internal-test path | PRESENT in progressive beginner example |
| Input-short path | PRESENT in progressive beginner example |
| 250-SPS gain-24 EEG path | PRESENT in progressive beginner example |
| Canonical packet stream | PRESENT in progressive beginner example |
| Clean continuous stop | PRESENT: STOP/SDATAC path in beginner example |
| Host smoke-test recipe | PRESENT under `tests/` |
| Legacy minimal example | PRESERVED |
| Core edits required for beginner board bring-up | NO |
| STM32CubeH5 target build evidence | NOT YET RECORDED |
| NUCLEO-H563ZI + ADS1299 hardware evidence | NOT YET RECORDED |
| Sustained EXTI/DMA loss accounting | NOT YET VERIFIED |
| Multi-ADS1299 / 64-channel board throughput | NOT YET VERIFIED |

For 8xADS1299/64-channel use, retain the repository architecture of shared SPI with independent CS unless a separately documented topology is intentionally validated. Do not infer 64-channel capability from MCU performance alone; validate DRDY service latency, bounded buffering, packet transport bandwidth and loss accounting on the target system.
