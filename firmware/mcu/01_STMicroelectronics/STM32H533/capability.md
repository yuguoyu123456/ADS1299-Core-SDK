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

## Current integration capabilities

The planning table above is retained for historical catalog compatibility. Current repository integration capabilities are:

| Area | Current state |
|---|---|
| Reference board | NUCLEO-H533RE / STM32H533RET6 |
| Board configuration | `board/board_config.h` is the single obvious starter configuration point |
| ADS1299 SPI contract | Mode 1, 8-bit, MSB-first, software CS |
| Raw ADS1299-8 frame | 27-byte shared-core contract |
| Beginner acquisition profile | 250 SPS, gain 24 via shared ADS1299 APIs |
| Beginner diagnostics | probe/ID, internal test, input short, DRDY/frame/stream failure separation |
| Canonical stream | shared packet encoder; platform does not reimplement packet format |
| Portability boundary | platform HAL/board layer only; shared ADS1299 register/model logic remains in core |
| Sustained acquisition design | short DRDY path + bounded static queue/ring buffer recommended; not yet hardware-verified |
| DMA/cache | project-specific integration requirement; not yet verified on the stated board |
| Multi-device | shared SPI + independent CS is the repository default architecture; H533 hardware validation not yet recorded |
| 64 channels | not verified on STM32H533 |

The presence of code describing a capability does not imply throughput, timing, long-run, electrical, or hardware validation. See `validation.md` for evidence state.
